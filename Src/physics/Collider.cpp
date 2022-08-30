#include "Collider.h"

#include "../objects/Object.h"

bool Face::test_sphere_collision(Bounding_Region& sphere, glm::vec3 c0, glm::vec3 c1, glm::vec3& ci) const {
    const float d0 = distance_to_point(sphere.center);
    const float d1 = distance_to_point(sphere.center);
    float u;

    // Check if the sphere intersects this frame
    if (fabsf(d0) <= sphere.radius) {
        ci = c0;
        return true;
    }

    // Check if the sphere intersects the next frame
    if (d0 > sphere.radius && d1 < sphere.radius) {
        u = (d0 - sphere.radius) / (d0 - d1);
        ci = (1 - u) * c0 + u * c1;
        return true;
    }

    return false;
}

void Collider::init(const std::shared_ptr<Object>& object, Bound_Type bound_type) {
    m_object = object;
    m_bound_type = bound_type;
    auto& mesh = m_object->m_model->m_meshes.front();
    m_positions.resize(mesh.m_vertices.size());
    m_faces.resize(mesh.m_indices.size() / 3);

    glm::vec3 min(m_object->m_rb->model * glm::vec4(mesh.m_vertices.front().position, 1.0f));
    glm::vec3 max = min;

    for (unsigned int i = 0; i < m_positions.size(); i++) {
        m_positions[i] = mesh.m_vertices.at(i).position;
        glm::vec3 transformed = m_object->m_rb->model * glm::vec4(mesh.m_vertices.at(i).position, 1.0f);
        for (int j = 0; j < 3; j++) {
            if (transformed[j] < min[j])
                min[j] = transformed[j];
            if (transformed[j] > max[j])
                max[j] = transformed[j];
        }
    }

    if (bound_type == Bound_Type::SPHERE_BV) {
        m_br = Bounding_Region(
            m_object->m_rb->position,
            std::max({ m_object->m_rb->scale[0], m_object->m_rb->scale[1], m_object->m_rb->scale[2] }) * 0.5f);
    } else {
        m_br = Bounding_Region(min, max);
    }

    m_br.rb = m_object->m_rb->shared_from_this();
    m_br.transform();

    for (unsigned int i = 0; i < m_faces.size(); i++) {
        glm::vec3 p0 = m_br.rb->model * glm::vec4(m_positions.at(mesh.m_indices.at(i * 3)), 1.0);
        glm::vec3 p1 = m_br.rb->model * glm::vec4(m_positions.at(mesh.m_indices.at(i * 3 + 1)), 1.0);
        glm::vec3 p2 = m_br.rb->model * glm::vec4(m_positions.at(mesh.m_indices.at(i * 3 + 2)), 1.0);

        m_faces[i] = { p0, p1, p2 };
    }
}

void Collider::recalculate() {
    glm::vec3 min(m_object->m_rb->model * glm::vec4(m_positions.front(), 1.0f));
    glm::vec3 max = min;

    for (auto& position: m_positions) {
        glm::vec3 transformed = m_object->m_rb->model * glm::vec4(position, 1.0f);
        for (int j = 0; j < 3; j++) {
            if (transformed[j] < min[j])
                min[j] = transformed[j];
            if (transformed[j] > max[j])
                max[j] = transformed[j];
        }
    }

    if (m_bound_type == Bound_Type::SPHERE_BV) {
        m_br.center = m_object->m_rb->position;
        m_br.radius = std::max({ m_object->m_rb->scale[0], m_object->m_rb->scale[1], m_object->m_rb->scale[2] }) * 0.5f;
    } else {
        m_br.min = min;
        m_br.max = max;
    }

    m_br.transform();

    auto& mesh = m_object->m_model->m_meshes.front();
    for (unsigned int i = 0; i < m_faces.size(); i++) {
        glm::vec3 p0 = m_br.rb->model * glm::vec4(m_positions.at(mesh.m_indices.at(i * 3)), 1.0);
        glm::vec3 p1 = m_br.rb->model * glm::vec4(m_positions.at(mesh.m_indices.at(i * 3 + 1)), 1.0);
        glm::vec3 p2 = m_br.rb->model * glm::vec4(m_positions.at(mesh.m_indices.at(i * 3 + 2)), 1.0);

        m_faces[i] = { p0, p1, p2 };
    }
}
