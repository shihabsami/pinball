#include "../utility/Global.h"

#include "Flipper.h"

void Flipper::init(float min_angle, float max_angle, const glm::vec3& swing_origin, bool left_rotate) {
    Object::init();
    m_angle = min_angle;
    m_min_angle = min_angle;
    m_max_angle = max_angle;
    m_left_rotate = left_rotate;
    m_swing_origin = swing_origin;
    m_rb->has_outward_force = true;
}

void Flipper::swing() {
    ascending = true;
}

void Flipper::update() {
    if (Global::dt_modded != 0.0f && Global::dt_modded < 1.0f) {
        m_rb->position += m_rb->velocity * Global::dt_modded + 0.5f * m_rb->acceleration * (Global::dt_modded * Global::dt_modded);
        m_rb->velocity += m_rb->acceleration * Global::dt_modded;
        m_rb->is_translated = m_rb->position != m_rb->last_position;
        if (m_rb->is_translated)
            m_rb->last_position = m_rb->position;
        m_rb->is_rotated = m_rb->rotation != m_rb->last_rotation;
        if (m_rb->is_rotated)
            m_rb->last_rotation = m_rb->rotation;
        m_rb->is_scaled = m_rb->scale != m_rb->last_scale;
        if (m_rb->is_scaled)
            m_rb->last_scale = m_rb->scale;

        m_rb->last_collision += Global::dt_modded;
    }

    if (ascending) {
        if (t == 1.0f)
            ascending = false;

        t = (t < 1.0f) ? t + Global::dt_modded * 5.0f : t;
        t = t > 1.0f ? 1.0f : t;
    } else {
        t = (t > 0.0f) ? t - Global::dt_modded : t;
        t = t < 0.0f ? 0.0f : t;
    }

    m_angle = glm::mix(m_min_angle, m_max_angle, t);

    m_rb->model = glm::translate(MAT4_IDENTITY, m_swing_origin);
    m_rb->rotation = glm::angleAxis(glm::radians(m_angle), m_left_rotate ? -VEC3_FORWARD : VEC3_FORWARD);
    m_rb->model = m_rb->model * glm::mat4_cast(m_rb->rotation);
    m_rb->model = glm::translate(m_rb->model, -m_swing_origin);

    m_rb->model = glm::translate(m_rb->model, m_rb->position);
    m_rb->model = glm::scale(m_rb->model, m_rb->scale);
    m_collider->recalculate();
}
