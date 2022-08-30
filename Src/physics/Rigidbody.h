#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include "../utility/Constants.h"

#include <string>
#include <memory>

#define COLLISION_THRESHOLD 0.05f

// Represents physical body and holds all parameters
class Rigidbody : public std::enable_shared_from_this<Rigidbody> {
public:
    explicit Rigidbody(
        glm::vec3 position = VEC3_IDENTITY,
        glm::quat rotation = QUAT_IDENTITY,
        glm::vec3 scale = glm::vec3(1.0f),
        float mass = 1.0f
    );

    void update(float dt);

    void update_matrix();

    void update_inclined();

    // Apply a force
    void add_force(const glm::vec3& force);

    // Apply an acceleration
    void add_acceleration(const glm::vec3& amount);

    // Transfer potential or kinetic energy from another object
    void transfer_energy(float joules, const glm::vec3& direction);

    void handle_collision(const std::shared_ptr<Rigidbody>& rb, const glm::vec3& normal);

    static std::string get_id() {
        static size_t new_id = 0;
        return std::to_string(new_id++);
    }

    std::string id;
    glm::vec3 position;
    glm::vec3 last_position;
    glm::quat rotation;
    glm::quat last_rotation;
    glm::vec3 scale;
    glm::vec3 last_scale;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    glm::mat4 model{ MAT4_IDENTITY };
    bool is_translated{};
    bool is_rotated{};
    bool is_scaled{};
    bool is_dead{};
    bool has_outward_force{};
    float mass;
    float last_collision;
    std::string last_collision_id;
};

#endif // RIGIDBODY_H
