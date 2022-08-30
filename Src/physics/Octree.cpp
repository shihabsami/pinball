#include "Octree.h"

#include "../components/Shader_Library.h"

Octree::Node::Node() = default;

Octree::Node::Node(Bounding_Region bounds) : m_region(std::move(bounds)) {}

Octree::Node::Node(Bounding_Region bounds, std::vector<std::shared_ptr<Object>>& objects)
    : m_region(std::move(bounds)) {
    for (auto& object: objects)
        m_objects.push_back(object->shared_from_this());
}

void Octree::Node::build() {
    glm::vec3 dimensions = m_region.calculate_dimensions();

    // Only a single object remain
    if (m_objects.size() <= 1) {
        for (auto& object: m_objects)
            object->m_collider->m_br.octant = shared_from_this();

        return;
    }

    // Dimensions of this node is smaller than minimum
    for (long i = 0; i < 3; i++) {
        if (dimensions[i] < MIN_BOUNDS) {
            for (auto& object: m_objects)
                object->m_collider->m_br.octant = shared_from_this();

            return;
        }
    }

    std::array<Bounding_Region, CHILDREN_COUNT> octants;
    std::array<std::vector<std::shared_ptr<Object>>, CHILDREN_COUNT> octant_objects;

    // Calculate the bounding regions for each of the eight octants
    for (long i = 0; i < CHILDREN_COUNT; i++)
        octants.at(i) = calculate_bounds((Octant) i, m_region);

    // One by one determine which octant to place objects in
    for (long i = 0, remaining = (long) m_objects.size(); i < remaining; i++) {
        Bounding_Region& br = m_objects.at(i)->m_collider->m_br;

        for (long j = 0; j < CHILDREN_COUNT; j++) {
            // If the octant fully contains the region
            if (octants.at(j).contains_region(br)) {
                octant_objects.at(j).push_back(m_objects.at(i));
                m_objects.erase(m_objects.begin() + i);

                i--;
                remaining--;
                break;
            }
        }
    }

    // Insert the objects deduced above into the octants
    for (long i = 0; i < CHILDREN_COUNT; i++) {
        if (!octant_objects.at(i).empty()) {
            m_children.at(i) = std::make_shared<Node>(octants.at(i), octant_objects.at(i));
            m_children.at(i)->m_parent = shared_from_this();
            m_children.at(i)->build();
            m_active.at(i) = true;
            has_children = true;
        }
    }

    for (auto& object: m_objects)
        object->m_collider->m_br.octant = shared_from_this();
}

Bounding_Region Octree::calculate_bounds(Octant octant, const Bounding_Region& parent_region) {
    // Calculate the bounding volume of the specified octant from centre, min and max
    glm::vec3 center = parent_region.calculate_center();
    switch (octant) {
    case TRF:
        return { center, parent_region.max };
    case TLF:
        return {
            glm::vec3(parent_region.min.x, center.y, center.z),
            glm::vec3(center.x, parent_region.max.y, parent_region.max.z)
        };
    case TRN:
        return {
            glm::vec3(center.x, center.y, parent_region.min.z),
            glm::vec3(parent_region.max.x, parent_region.max.y, center.z)
        };
    case TLN:
        return {
            glm::vec3(parent_region.min.x, center.y, parent_region.min.z),
            glm::vec3(center.x, parent_region.max.y, center.z)
        };
    case BRF:
        return {
            glm::vec3(center.x, parent_region.min.y, center.z),
            glm::vec3(parent_region.max.x, center.y, parent_region.max.z)
        };
    case BLF:
        return {
            glm::vec3(parent_region.min.x, parent_region.min.y, center.z),
            glm::vec3(center.x, center.y, parent_region.max.z)
        };
    case BRN:
        return {
            glm::vec3(center.x, parent_region.min.y, parent_region.min.z),
            glm::vec3(parent_region.max.x, center.y, center.z)
        };
    case BLN:
        return { parent_region.min, center };
    default:
        return parent_region;
    }
}

bool Octree::Node::insert(std::shared_ptr<Object>& object) {
    // Empty leaf node or dimensions are smaller than minimum
    glm::vec3 dimensions = m_region.calculate_dimensions();
    if (m_objects.empty() ||
        dimensions.x < MIN_BOUNDS ||
        dimensions.y < MIN_BOUNDS ||
        dimensions.z < MIN_BOUNDS) {
        object->m_collider->m_br.octant = shared_from_this();
        m_objects.push_back(object);
        return true;
    }

    // Move object up the tree
    if (!m_region.contains_region(object->m_collider->m_br))
        return m_parent && m_parent->insert(object);

    std::array<Bounding_Region, CHILDREN_COUNT> octants;
    std::array<std::vector<std::shared_ptr<Object>>, CHILDREN_COUNT> octant_objects;
    m_objects.push_back(object);

    // Calculate region if not already calculated
    for (long i = 0; i < CHILDREN_COUNT; i++) {
        if (m_children.at(i)) {
            // Child exists, reuse
            octants.at(i) = m_children.at(i)->m_region;
        } else {
            // Re-calculate region
            octants.at(i) = calculate_bounds((Octant) i, m_region);
        }
    }

    // One by one determine which octant to place objects in
    for (long i = 0, remaining = (long) m_objects.size(); i < remaining; i++) {
        Bounding_Region& br = m_objects.at(i)->m_collider->m_br;
        br.octant = shared_from_this();

        for (long j = 0; j < CHILDREN_COUNT; j++) {

            // If the octant fully contains the region
            if (octants.at(j).contains_region(br)) {
                octant_objects.at(j).push_back(m_objects.at(i));
                m_objects.erase(m_objects.begin() + i);

                i--;
                remaining--;
                break;
            }
        }
    }

    // Populate octants
    for (long i = 0; i < CHILDREN_COUNT; i++) {
        if (!octant_objects.at(i).empty()) {
            // Objects exist in this octant
            if (m_children.at(i)) {
                for (auto& octant_object: octant_objects.at(i))
                    m_children.at(i)->insert(octant_object);
            } else {
                // Create new node
                m_children.at(i) = std::make_shared<Node>(octants.at(i), octant_objects.at(i));
                m_children.at(i)->m_parent = shared_from_this();
                m_children.at(i)->build();
                m_active.at(i) = true;
                has_children = true;
            }
        }
    }

    return true;
}

void Octree::Node::render() {
    glColor3f(1.0f, 0.0f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    for (auto&[a, b, c]: Global::debug_lines) {
        glVertex3fv(glm::value_ptr(a));
        glVertex3fv(glm::value_ptr(b));
    }
    glLineWidth(1.0f);
    glEnd();

    m_region.render();
    for (auto& object: m_objects)
        object->render();

    for (long i = 0; i < CHILDREN_COUNT; i++)
        if (m_children.at(i))
            m_children.at(i)->render();
}

void Octree::Node::render(std::shared_ptr<Shader>& shader) {
    for (auto& object: m_objects) {
        if (object->m_collider->m_br.type == AABB) {
            object->render(shader);
        } else {
            object->render(Shader_Library::shaders.at(ENVIRONMENT_MAPPED));
        }
    }

    for (int i = 0; i < CHILDREN_COUNT; i++)
        if (m_children.at(i))
            m_children.at(i)->render(shader);
}

void Octree::Node::update(std::shared_ptr<Particle_System>& particle_system) {
    for (long i = 0, remaining = (long) Global::debug_lines.size(); i < remaining; i++) {
        auto& timer = std::get<2>(Global::debug_lines.at(i));
        timer -= Global::dt;
        if (timer < 0.0f) {
            Global::debug_lines.erase(Global::debug_lines.begin() + i);

            i--;
            remaining--;
        }
    }

    // Countdown timer
    if (m_objects.empty() && !has_children) {
        if (current_lifespan == -1)
            current_lifespan = max_lifespan;
        else if (current_lifespan > 0)
            current_lifespan--;
    } else {
        if (current_lifespan != -1)
            if (max_lifespan <= 64)
                max_lifespan += 2;
    }

    // Delete objects marked as dead
    for (long i = 0, remaining = (long) m_objects.size(); i < remaining; i++) {
        if (m_objects.at(i)->m_rb->is_dead) {
            m_objects.erase(m_objects.begin() + i);

            i--;
            remaining--;
        }
    }

    std::stack<long> moved_objects;
    // Get moved objects that were in this leaf in previous frame
    for (long i = 0; i < (long) m_objects.size(); i++) {
        auto& object = m_objects.at(i);
        object->update();

        if (object->m_rb->is_translated) {
            object->m_rb->is_translated = false;
            moved_objects.push(i);
        }

        if (object->m_rb->is_rotated ||
            object->m_rb->is_scaled) {
            object->m_rb->is_rotated = false;
            object->m_rb->is_scaled = false;
        }
    }

    // Remove dead nodes
    for (long i = 0; i < CHILDREN_COUNT; i++) {
        if (m_active.at(i) && m_children.at(i) && m_children.at(i)->current_lifespan == 0) {
            if (!m_children.at(i)->m_objects.empty()) {
                m_children.at(i)->current_lifespan = -1;
            } else {
                m_children.at(i) = nullptr;
                m_active.at(i) = false;
            }
        }
    }

    // Update all child nodes
    for (long i = 0; i < CHILDREN_COUNT; i++)
        if (m_active.at(i) && m_children.at(i))
            m_children.at(i)->update(particle_system);

    // Move moved objects into updated nodes
    while (!moved_objects.empty()) {
        auto object = m_objects.at(moved_objects.top());
        auto current = shared_from_this();

        // Traverse up tree until a region is found that completely encloses the object
        while (!current->m_region.contains_region(object->m_collider->m_br)) {
            if (current->m_parent)
                current = current->m_parent;
            else
                break;
        }

        // Push object as far down as possible
        m_objects.erase(m_objects.begin() + moved_objects.top());
        moved_objects.pop();
        current->insert(object);
        current = object->m_collider->m_br.octant;

        // Check collisions with all objects in this node
        current->check_collisions_self(object, particle_system);

        // Check collisions with all objects in child nodes
        current->check_collisions_children(object, particle_system);

        // Traverse up
        while (current->m_parent) {
            current = current->m_parent;
            current->check_collisions_self(object, particle_system);
        }
    }
}

void Octree::Node::mark_dead_objects(std::shared_ptr<Octree::Node>& node, const Bounding_Region& maximum_region) {
    for (auto& object : m_objects)
        if (!maximum_region.contains_region(object->m_collider->m_br))
            object->m_rb->is_dead = true;

    for (int i = 0; i < CHILDREN_COUNT; i++)
        if (m_active.at(i) && m_children.at(i))
            mark_dead_objects(m_children.at(i), maximum_region);
}

void Octree::Node::check_collisions_self(
    std::shared_ptr<Object>& self, std::shared_ptr<Particle_System>& particles_system) {
    for (auto& object: m_objects) {
        // Do not test collisions with the same instance
        if (object->m_rb->id == self->m_rb->id)
            continue;

        // Coarse check for bounding region intersection
        if (object->m_collider->m_br.intersects_with(self->m_collider->m_br)) {
            // AABB vs Sphere
            if (object->m_bound_type == AABB && self->m_bound_type == SPHERE_BV) {
                for (auto& o_face: object->m_collider->m_faces) {
                    glm::vec3 c0, c1, ci;
                    c0 = self->m_rb->position;
                    c1 = self->m_rb->position + self->m_rb->velocity * Global::dt_modded
                        + 0.5f * self->m_rb->acceleration * (Global::dt_modded * Global::dt_modded);

                    if (o_face.test_sphere_collision(self->m_collider->m_br, c0, c1, ci)) {
                        self->m_rb->position += o_face.N * 2.0f * glm::distance(c0, c1);
                        self->m_rb->handle_collision(object->m_rb, o_face.N);
                        particles_system->add(
                            self->m_collider->m_br.center + (o_face.N * self->m_collider->m_br.radius));

                        if (object->m_rb->has_outward_force) {
                            self->m_rb->transfer_energy(50.0f, o_face.N);
                        }

                        glm::vec3 T = { -o_face.N.y, o_face.N.x, o_face.N.z };
                        Global::debug_lines.emplace_back(self->m_rb->position, self->m_rb->position + o_face.N * 5.0f, 4.0f);
                        Global::debug_lines.emplace_back(self->m_rb->position, self->m_rb->position + T * 5.0f, 4.0f);
                        break;
                    }
                }
            }
                // Sphere vs Sphere
            else if (object->m_bound_type == SPHERE_BV && self->m_bound_type == SPHERE_BV) {
                glm::vec3 normal = self->m_collider->m_br.center - object->m_collider->m_br.center;
                self->m_rb->handle_collision(object->m_rb, normal);
                particles_system->add(
                    self->m_collider->m_br.center + (glm::normalize(normal) * self->m_collider->m_br.radius));

                glm::vec3 N = glm::normalize(normal);
                glm::vec3 T = { -N.y, N.x, N.z };
                Global::debug_lines.emplace_back(self->m_rb->position, self->m_rb->position + N * 5.0f, 4.0f);
                Global::debug_lines.emplace_back(self->m_rb->position, self->m_rb->position + T * 5.0f, 4.0f);
            }
        }
    }
}

void Octree::Node::check_collisions_children(
    std::shared_ptr<Object>& object, std::shared_ptr<Particle_System>& particles_system) {
    // Check collisions with all objects in child nodes
    for (long i = 0; i < CHILDREN_COUNT; i++) {
        if (m_active.at(i) && m_children.at(i)) {
            m_children.at(i)->check_collisions_self(object, particles_system);
            m_children.at(i)->check_collisions_children(object, particles_system);
        }
    }
}

void Octree::Node::destroy() {
    // Destroy child subtrees
    for (long i = 0; i < CHILDREN_COUNT; i++) {
        if (m_children.at(i)) {
            m_children.at(i)->destroy();
            m_children.at(i).reset();
        }
    }

    // Clear this node
    std::for_each(m_objects.begin(), m_objects.end(), [&](std::shared_ptr<Object>& object) {
        object.reset();
    });
    m_objects.clear();
}
