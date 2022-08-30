#ifndef OBJECT_H
#define OBJECT_H

#include "../physics/Bounds.h"
#include "../physics/Collider.h"
#include "../components/Model.h"

class Object : public std::enable_shared_from_this<Object> {
public:
    Object() = default;

    Object(
        std::shared_ptr<Model>  model,
        const glm::vec3& position,
        const glm::quat& rotation,
        const glm::vec3& scale,
        float mass,
        Bound_Type bound_type);

    virtual ~Object();

    virtual void init();

    virtual void update();

    virtual void render();

    virtual void render(std::shared_ptr<Shader>& shader);

    std::shared_ptr<Model> m_model;
    std::shared_ptr<Rigidbody> m_rb;
    std::shared_ptr<Collider> m_collider;
    Bound_Type m_bound_type{ AABB };
    bool m_render_enabled{ true };
};

#endif // OBJECT_H
