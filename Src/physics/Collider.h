#ifndef COLLISION_MESH_H
#define COLLISION_MESH_H

#include "Bounds.h"

#include <vector>

class Object;

class Face {
public:
    Face() = default;

    Face(const glm::vec3& p0, const glm::vec3& n) : N(n), D(glm::dot(-N, p0)) {}

    Face(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2)
        : N(glm::normalize(glm::cross(p1 - p0, p2 - p0))), D(glm::dot(-N, p0)) {}

    [[nodiscard]] float distance_to_point(const glm::vec3& p) const {
        return glm::dot(N, p) + D;
    }

    bool test_sphere_collision(Bounding_Region& sphere, glm::vec3 c0, glm::vec3 c1, glm::vec3& ci) const;

    glm::vec3 N{};
    float D{};
};

class Collider : public std::enable_shared_from_this<Collider> {
public:
    Collider() = default;

    void init(const std::shared_ptr<Object>& object, Bound_Type bound_type);

    void recalculate();

    Bounding_Region m_br;
    std::shared_ptr<Object> m_object;
    std::vector<glm::vec3> m_positions;
    std::vector<Face> m_faces;
    Bound_Type m_bound_type{ AABB };
};

#endif // COLLISION_MESH_H
