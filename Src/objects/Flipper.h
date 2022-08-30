#ifndef PADDLE_H
#define PADDLE_H

#include "Object.h"

class Flipper : public Object {
public:
    using Object::Object;

    void init(float min_angle, float max_angle, const glm::vec3& swing_origin, bool left_rotate);

    void update() override;

    void swing();

    float t{};
    bool ascending{};
    float m_angle{};
    float m_min_angle{};
    float m_max_angle{};
    bool m_left_rotate;
    glm::vec3 m_swing_origin;
};

#endif // PADDLE_H
