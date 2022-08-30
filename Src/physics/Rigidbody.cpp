#include "Rigidbody.h"

#include "../utility/Debug.h"
#include "../utility/Global.h"

Rigidbody::Rigidbody(
    glm::vec3 position,
    glm::quat rotation,
    glm::vec3 scale,
    float mass)
    :
    id(get_id()),
    position(position),
    last_position(position),
    rotation(rotation),
    last_rotation(rotation),
    scale(scale),
    last_scale(scale),
    mass(mass),
    velocity(0.0f),
    acceleration(0.0f),
    last_collision(COLLISION_THRESHOLD) {
    update(0.0f);
    update_matrix();
}

void Rigidbody::update(float dt) {
    if (dt != 0.0f && dt < 1.0f) {
        position += velocity * dt + 0.5f * acceleration * (dt * dt);
        velocity += acceleration * dt;
        is_translated = position != last_position;
        if (is_translated)
            last_position = position;
        is_rotated = rotation != last_rotation;
        if (is_rotated)
            last_rotation = rotation;
        is_scaled = scale != last_scale;
        if (is_scaled)
            last_scale = scale;

        last_collision += dt;
    }
}

void Rigidbody::update_matrix() {
    model = glm::translate(MAT4_IDENTITY, position);
    model = model * glm::mat4_cast(rotation);
    model = glm::scale(model, scale);
    glm::transpose(glm::inverse(glm::mat3(model)));
}

void Rigidbody::update_inclined() {
    rotation = QUAT_IDENTITY * glm::angleAxis(glm::radians(Global::incline), VEC3_RIGHT);
    model = MAT4_IDENTITY * glm::mat4_cast(rotation);
    model = glm::translate(model, position);
    model = glm::scale(model, scale);
    glm::transpose(glm::inverse(glm::mat3(model)));
}

void Rigidbody::add_force(const glm::vec3& force) {
    acceleration += force / mass;
}

void Rigidbody::add_acceleration(const glm::vec3& amount) {
    acceleration += amount;
}

void Rigidbody::transfer_energy(float joules, const glm::vec3& direction) {
    if (joules == 0)
        return;

    // KE = 1/2 * m * v^2
    glm::vec3 deltaV = sqrtf(2 * abs(joules) / mass) * direction;
    velocity += joules > 0 ? deltaV : -deltaV;
}

void Rigidbody::handle_collision(const std::shared_ptr<Rigidbody>& rb, const glm::vec3& normal) {
    if (last_collision >= COLLISION_THRESHOLD || last_collision_id != rb->id) {
        velocity = glm::reflect(velocity, glm::normalize(normal)) * 0.9f;
        last_collision = 0.0f;
    }

    last_collision_id = rb->id;
}
