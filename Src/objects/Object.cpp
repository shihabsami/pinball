#include "Object.h"

Object::Object(
    std::shared_ptr<Model> model,
    const glm::vec3& position = VEC3_IDENTITY,
    const glm::quat& rotation = QUAT_IDENTITY,
    const glm::vec3& scale = glm::vec3{ 1.0f },
    float mass = 1.0f,
    Bound_Type bound_type = AABB)
    :
    m_model(std::move(model)),
    m_rb(std::make_shared<Rigidbody>(position, rotation, scale, mass)),
    m_collider(std::make_shared<Collider>()),
    m_bound_type(bound_type) {}

void Object::init() {
    m_collider->init(shared_from_this(), m_bound_type);
}

void Object::render() {
    if (m_render_enabled) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glMultMatrixf(glm::value_ptr(m_rb->model));
        m_model->render();
        glPopMatrix();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 1.0f);
    glLineWidth(4.0f);
    glVertex3fv(glm::value_ptr(m_collider->m_br.center));
    glVertex3fv(glm::value_ptr(m_collider->m_br.center + (glm::normalize(m_rb->velocity) * 5.0f)));
    glEnd();
    m_collider->m_br.render();
}

void Object::render(std::shared_ptr<Shader>& shader) {
    if (m_render_enabled) {
        shader->use();
        shader->set_mat4(MODEL_MATRIX, m_rb->model);
        m_model->render(shader);
    }
}

void Object::update() {
    m_rb->update(Global::dt_modded);
    if (m_bound_type == AABB) {
        m_rb->update_inclined();
    } else {
        m_rb->update_matrix();
    }
    m_collider->recalculate();
}

Object::~Object() {
    m_rb.reset();
    m_collider.reset();
}
