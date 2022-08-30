#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Camera {
public:
    Camera(float y_fov, float aspect_ratio, float z_near, float z_far);

    void translate(const glm::vec3& position);

    void rotate(const glm::quat& orientation);

    void update();

    void pitch(float angle);

    void roll(float angle);

    void yaw(float angle);

    glm::vec3 m_position;
    glm::vec3 m_forward;
    glm::vec3 m_right;
    glm::vec3 m_up;
    glm::quat m_orientation;
    glm::mat4 m_view_matrix;
    glm::mat4 m_projection_matrix;
};

#endif // CAMERA_H
