#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include "../physics/Rigidbody.h"
#include "Lighting.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <random>
#include <utility>
#include <vector>
#include <unordered_map>

constexpr unsigned int PARTICLE_GROUP_SIZE = 10;

namespace Octree {
    class Node;
}

class Particle {
public:
    Particle() = default;

    Particle(float life, Rigidbody rigidbody) : life(life), rigidbody(std::move(rigidbody)) {}

    float life{};
    Rigidbody rigidbody;
};

class Particle_Group {
public:
    std::array<Particle, PARTICLE_GROUP_SIZE> particles{};
    std::array<glm::vec4, PARTICLE_GROUP_SIZE> transforms{};
};

class Particle_System {
public:
    void init(std::shared_ptr<Lighting_Model>& model);

    void add(const glm::vec3& position);

    void update(std::shared_ptr<Octree::Node>& octree);

    void render();

    void done();

    unsigned int VAO, VBO, texture;
    std::shared_ptr<Lighting_Model> lighting_model;
    std::vector<Particle_Group> groups;
    std::random_device rd;
    std::mt19937 mt{ rd() };
    std::uniform_real_distribution<float> distribution{ -1.0f, 1.0f };
};

#endif // PARTICLE_SYSTEM_H
