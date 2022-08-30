#ifndef BOUNDS_H
#define BOUNDS_H

#include "../physics/Rigidbody.h"
#include "../components/Mesh.h"

#include <glm/glm.hpp>

#include <array>

class Collider;

namespace Octree {
    class Node;
}

enum Bound_Type {
    AABB,
    SPHERE_BV
};

class Bounding_Region {
public:
    explicit Bounding_Region(Bound_Type type = Bound_Type::AABB);

    // Initialize as AABB
    Bounding_Region(const glm::vec3& min, const glm::vec3& max);

    // Initialize as Sphere
    Bounding_Region(const glm::vec3& center, float radius);

    void init_cube_mesh();

    void update_cube_vertices();

    void init_sphere_mesh();

    void update_sphere_transformation();

    void assign_random_color();

    void transform();

    [[nodiscard]] glm::vec3 calculate_center() const;

    [[nodiscard]] glm::vec3 calculate_dimensions() const;

    [[nodiscard]] bool contains_point(const glm::vec3& point) const;

    [[nodiscard]] bool contains_region(const Bounding_Region& br) const;

    [[nodiscard]] bool intersects_with(const Bounding_Region& br) const;

    void render() const;

    Bound_Type type;
    std::shared_ptr<Octree::Node> octant{};
    std::shared_ptr<Rigidbody> rb{};
    glm::vec3 color{};
    std::vector<glm::vec3> vertices;
    std::vector<glm::uvec4> indices;
    glm::mat4 transformation_matrix{ MAT4_IDENTITY };

    // AABB
    glm::vec3 min{};
    glm::vec3 max{};

    // Sphere
    glm::vec3 center{};
    float radius{};
};

#endif // BOUNDS_H
