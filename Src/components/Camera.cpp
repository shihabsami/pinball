#include "Camera.h"

#include "../utility/Constants.h"

Camera::Camera(float y_fov, float aspect_ratio, float z_near, float z_far) :
    m_forward(VEC3_FORWARD),
    m_right(VEC3_RIGHT),
    m_up(VEC3_UP),
    m_position(VEC3_IDENTITY),
    m_orientation(QUAT_IDENTITY),
    m_view_matrix(MAT4_IDENTITY),
    m_projection_matrix(glm::perspective(glm::radians(y_fov), aspect_ratio, z_near, z_far)) {}

void Camera::translate(const glm::vec3& position) {
    m_position = position;

    update();
}

void Camera::rotate(const glm::quat& orientation) {
    m_orientation = m_orientation * orientation;
    m_forward = m_orientation * VEC3_FORWARD;
    m_right = m_orientation * VEC3_RIGHT;
    m_up = m_orientation * VEC3_UP;

    update();
}

void Camera::pitch(float angle) {
    rotate(glm::angleAxis(glm::radians(angle), VEC3_RIGHT));
}

void Camera::roll(float angle) {
    rotate(glm::angleAxis(glm::radians(angle), VEC3_FORWARD));
}

void Camera::yaw(float angle) {
    rotate(glm::angleAxis(glm::radians(angle), VEC3_UP));
}

void Camera::update() {
    m_view_matrix = glm::translate(MAT4_IDENTITY, m_position * -1.0f);
    m_view_matrix = glm::mat4_cast(glm::inverse(m_orientation)) * m_view_matrix;
}
