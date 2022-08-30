#ifndef OCTREE_H
#define OCTREE_H

constexpr unsigned int CHILDREN_COUNT = 8;
constexpr float MIN_BOUNDS = 0.5f;

#include "Bounds.h"
#include "../objects/Object.h"
#include "../components/Particle_System.h"

#include <queue>
#include <stack>
#include <vector>

namespace Octree {
    // Enum to represent each octant
    enum Octant {
        TRF, TLF, TRN, TLN, BRF, BLF, BRN, BLN
    };

    class Node : public std::enable_shared_from_this<Node> {
    public:
        Node();

        explicit Node(Bounding_Region bounds);

        Node(Bounding_Region bounds, std::vector<std::shared_ptr<Object>>& objects);

        void build();

        bool insert(std::shared_ptr<Object>& object);

        void render();

        void render(std::shared_ptr<Shader>& shader);

        void update(std::shared_ptr<Particle_System>& particle_system);

        void mark_dead_objects(std::shared_ptr<Octree::Node>& node, const Bounding_Region& maximum_region);

        // Check collisions with all objects in node
        void check_collisions_self(
            std::shared_ptr<Object>& self, std::shared_ptr<Particle_System>& particles_system);

        // Check collisions with all objects in child nodes
        void check_collisions_children(
            std::shared_ptr<Object>& object, std::shared_ptr<Particle_System>& particles_system);

        void destroy();

        Bounding_Region m_region;
        std::shared_ptr<Node> m_parent{};
        std::array<std::shared_ptr<Node>, CHILDREN_COUNT> m_children{};
        std::array<bool, CHILDREN_COUNT> m_active{};
        std::vector<std::shared_ptr<Object>> m_objects;

        bool has_children = false;
        int max_lifespan = 8;
        int current_lifespan = -1;
    };

    // Calculate bounds of specified octant in parent's bounding region
    Bounding_Region calculate_bounds(Octant octant, const Bounding_Region& parent_region);
}

#endif // OCTREE_H
