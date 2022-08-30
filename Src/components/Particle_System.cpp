#include "Particle_System.h"

#include "../physics/Octree.h"
#include "../utility/Loader.h"

void Particle_System::init(std::shared_ptr<Lighting_Model>& model) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    std::array<glm::vec4, PARTICLE_GROUP_SIZE> transforms{};
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * PARTICLE_GROUP_SIZE, transforms.data(), GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*) nullptr);
    glVertexAttribDivisor(0, 1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    lighting_model = model;
    texture = load_texture("particle-spark.png", "Src/assets/textures", false);
}

void Particle_System::add(const glm::vec3& position) {
    // Create group with 10 particles
    Particle_Group& pg = groups.emplace_back(Particle_Group());
    for (int i = 0; i < PARTICLE_GROUP_SIZE; i++) {
        Particle p = { 1.0f,
            Rigidbody(position, QUAT_IDENTITY, glm::vec3(distribution(mt) + 1.0f) * 0.5f, 1.0f) };
        p.rigidbody.add_acceleration(GRAVITY);
        p.rigidbody.transfer_energy(25.0f,
            glm::vec3(distribution(mt), distribution(mt), distribution(mt)));

        pg.particles.at(i) = p;
        pg.transforms.at(i) = glm::vec4(p.rigidbody.position, glm::length(p.rigidbody.scale) * p.life);
    }

    if (lighting_model->m_light_count < MAX_LIGHTS)
        lighting_model->add_point_light(
            Point_Light{ position, glm::vec3(0.8f, 0.3f, 0.3f),
                false, true, false });
}

void Particle_System::render() {
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    for (auto& pg: groups) {
        void* buffer_location = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        memcpy(buffer_location, pg.transforms.data(), sizeof(glm::vec4) * PARTICLE_GROUP_SIZE);
        glUnmapBuffer(GL_ARRAY_BUFFER);

        glDrawArraysInstanced(GL_POINTS, 0, 4, PARTICLE_GROUP_SIZE);
    }
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Particle_System::update(std::shared_ptr<Octree::Node>& octree) {
    for (size_t i = 0, remaining = groups.size(); i < remaining; i++) {
        auto& pg = groups.at(i);
        bool dead = true;

        for (size_t j = 0; j < PARTICLE_GROUP_SIZE; j++) {
            auto& p = pg.particles.at(j);
            p.life -= Global::dt_modded * 0.5f;
            p.life = !octree->m_region.contains_point(p.rigidbody.position) ? 0.0f : p.life;
            p.rigidbody.update(Global::dt_modded);
            pg.transforms.at(j) = glm::vec4(p.rigidbody.position, glm::length(p.rigidbody.scale) * p.life);
            dead &= (p.life < 0.0f);
        }

        std::sort(pg.particles.begin(), pg.particles.end(), [](Particle& p1, Particle& p2) {
            return p1.rigidbody.position.z > p2.rigidbody.position.z;
        });

        if (dead) {
            groups.erase(groups.begin() + (int) i);
            i--;
            remaining--;
        }
    }
}

void Particle_System::done() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}
