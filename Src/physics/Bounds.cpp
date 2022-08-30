#include "Bounds.h"

#include "../components/Model.h"
#include "../components/Model_Library.h"

#include <random>

Bounding_Region::Bounding_Region(Bound_Type type) : type(type) {
    assign_random_color();
}

Bounding_Region::Bounding_Region(const glm::vec3& min, const glm::vec3& max)
    : type(Bound_Type::AABB), min(min), max(max) {
    init_cube_mesh();
    assign_random_color();
}

Bounding_Region::Bounding_Region(const glm::vec3& center, float radius)
    : type(Bound_Type::SPHERE_BV), center(center), radius(radius) {
    init_sphere_mesh();
    update_sphere_transformation();
    assign_random_color();
}

void Bounding_Region::init_cube_mesh() {
    vertices.resize(8);
    update_cube_vertices();

    indices.resize(6);
    indices.at(0) = glm::uvec4(0, 1, 2, 3);
    indices.at(1) = glm::uvec4(1, 5, 6, 2);
    indices.at(2) = glm::uvec4(5, 4, 7, 6);
    indices.at(3) = glm::uvec4(4, 0, 3, 7);
    indices.at(4) = glm::uvec4(0, 1, 5, 4);
    indices.at(5) = glm::uvec4(2, 3, 7, 6);
}

void Bounding_Region::init_sphere_mesh() {
    std::shared_ptr<Model> model = Model_Library::at(BV_SPHERE_MODEL);
    vertices.resize(model->m_meshes.front().m_vertices.size());
    for (size_t i = 0; i < vertices.size(); i++)
        vertices.at(i) = model->m_meshes.front().m_vertices.at(i).position;

    indices.resize(model->m_meshes.front().m_indices.size() / 3);
    for (size_t i = 0; i < indices.size(); i++) {
        indices.at(i) = {
            model->m_meshes.front().m_indices.at(i * 3),
            model->m_meshes.front().m_indices.at(i * 3 + 1),
            model->m_meshes.front().m_indices.at(i * 3 + 2),
            0
        };
    }
}

void Bounding_Region::update_cube_vertices() {
    vertices.at(0) = { min.x, min.y, max.z };
    vertices.at(1) = { max.x, min.y, max.z };
    vertices.at(2) = { max.x, max.y, max.z };
    vertices.at(3) = { min.x, max.y, max.z };
    vertices.at(4) = { min.x, min.y, min.z };
    vertices.at(5) = { max.x, min.y, min.z };
    vertices.at(6) = { max.x, max.y, min.z };
    vertices.at(7) = { min.x, max.y, min.z };
}

void Bounding_Region::update_sphere_transformation() {
    transformation_matrix = glm::translate(MAT4_IDENTITY, center);
    transformation_matrix = glm::scale(transformation_matrix, glm::vec3(radius * 2.0f));
}

void Bounding_Region::assign_random_color() {
    std::random_device rd;
    std::mt19937_64 mt(rd());
    std::uniform_real_distribution distribution(0.0f, 1.0f);
    color = glm::vec3(distribution(mt), distribution(mt), distribution(mt));
}

void Bounding_Region::transform() {
    if (rb) {
        if (type == Bound_Type::AABB)
            update_cube_vertices();
        else
            update_sphere_transformation();
    }
}

void Bounding_Region::render() const {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glColor3fv(glm::value_ptr(color));
    glLineWidth(2.0f);

    if (type == Bound_Type::SPHERE_BV) {
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glMultMatrixf(glm::value_ptr(transformation_matrix));
        glBegin(GL_TRIANGLES);
        for (const auto& index: indices) {
            glVertex3fv(glm::value_ptr(vertices.at(index[0])));
            glVertex3fv(glm::value_ptr(vertices.at(index[1])));
            glVertex3fv(glm::value_ptr(vertices.at(index[2])));
        }
        glEnd();
        glPopMatrix();
    } else {
        glBegin(GL_QUADS);
        for (const auto& index: indices) {
            glVertex3fv(glm::value_ptr(vertices.at(index[0])));
            glVertex3fv(glm::value_ptr(vertices.at(index[1])));
            glVertex3fv(glm::value_ptr(vertices.at(index[2])));
            glVertex3fv(glm::value_ptr(vertices.at(index[3])));
        }
        glEnd();
    }

    glLineWidth(1.0f);
    glColor3fv(glm::value_ptr(VEC3_IDENTITY));
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

glm::vec3 Bounding_Region::calculate_center() const {
    return (type == Bound_Type::AABB) ? (min + max) / 2.0f : center;
}

glm::vec3 Bounding_Region::calculate_dimensions() const {
    return (type == Bound_Type::AABB) ? (max - min) : glm::vec3(2.0f * radius);
}

bool Bounding_Region::contains_point(const glm::vec3& point) const {
    if (type == Bound_Type::AABB) {
        // box - point must be larger than min and smaller than max
        return (point.x >= min.x) && (point.x <= max.x)
            && (point.y >= min.y) && (point.y <= max.y)
            && (point.z >= min.z) && (point.z <= max.z);
    } else {
        // x^2 + y^2 + z^2 <= r^2
        float d_squared = 0.0f;
        for (int i = 0; i < 3; i++)
            d_squared += (center[i] - point[i]) * (center[i] - point[i]);

        return d_squared <= (radius * radius);
    }
}

bool Bounding_Region::contains_region(const Bounding_Region& br) const {
    if (br.type == Bound_Type::AABB) {
        // Only has to contain min and max
        return contains_point(br.min) && contains_point(br.max);
    } else if (type == Bound_Type::SPHERE_BV && br.type == Bound_Type::SPHERE_BV) {
        // Combination of distance from centers and br.radius is less than radius
        return glm::length(center - br.center) + br.radius < radius;
    } else {
        if (!contains_point(br.center))
            return false;

        // Center inside the box
        for (int i = 0; i < 3; i++) {
            if (abs(max[i] - br.center[i]) < br.radius ||
                abs(br.center[i] - min[i]) < br.radius) {
                return false;
            }
        }

        return true;
    }
}

bool Bounding_Region::intersects_with(const Bounding_Region& br) const {
    // Determine if region intersects
    if (type == Bound_Type::AABB && br.type == Bound_Type::AABB) {
        glm::vec3 r = calculate_dimensions() / 2.0f;
        glm::vec3 r_br = br.calculate_dimensions() / 2.0f;
        glm::vec3 box_center = calculate_center();
        glm::vec3 br_center = br.calculate_center();
        glm::vec3 d = abs(box_center - br_center);

        for (int i = 0; i < 3; i++) {
            if (d[i] > r[i] + r_br[i]) {
                // no overlap on this axis
                return false;
            }
        }

        return true;
    } else if (type == Bound_Type::SPHERE_BV && br.type == Bound_Type::SPHERE_BV) {
        // Distance between centers must be less than combined radius
        glm::vec3 d = center - br.center;
        float d_squared = 0.0f;
        for (int i = 0; i < 3; i++)
            d_squared += d[i] * d[i];

        float r_squared = radius + br.radius;
        r_squared *= r_squared;

        return d_squared <= r_squared;
    } else if (type == Bound_Type::SPHERE_BV) {
        float d_squared = 0.0f;
        for (int i = 0; i < 3; i++) {
            // Determine closest side
            float closest = std::max(br.min[i], std::min(center[i], br.max[i]));
            // Add squared distance
            d_squared += (closest - center[i]) * (closest - center[i]);
        }

        return d_squared < (radius * radius);
    } else {
        return br.intersects_with(*this);
    }
}
