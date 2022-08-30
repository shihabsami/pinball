#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

constexpr unsigned int FRAME_RATE_INTERVAL = 100;

constexpr unsigned int SCREEN_WIDTH = 1920;
constexpr unsigned int SCREEN_HEIGHT = 1080;

constexpr float PROJECTION_FOV = 45.0f;
constexpr float PROJECTION_Z_NEAR = 0.1f;
constexpr float PROJECTION_Z_FAR = 100.0f;

constexpr float CAMERA_MOVE_FACTOR = 20.0f;
constexpr float CAMERA_ROTATION_FACTOR = 90.0f;
constexpr float MOUSE_SENSITIVITY = 0.001f;

constexpr unsigned int MAX_LIGHTS = 20;
constexpr unsigned int SHADOW_MAP_WIDTH = 4096;
constexpr unsigned int SHADOW_MAP_HEIGHT = 4096;
constexpr float DEPTH_PROJECTION_FOV = 90.0f;
constexpr float DEPTH_PROJECTION_CLIP_SIZE = 100.0f;
constexpr float DEPTH_PROJECTION_Z_NEAR = 1.0f;
constexpr float DEPTH_PROJECTION_Z_FAR = 100.0f;

constexpr glm::vec3 VEC3_FORWARD{ 0.0f, 0.0f, -1.0f };
constexpr glm::vec3 VEC3_RIGHT{ 1.0f, 0.0f, 0.0f };
constexpr glm::vec3 VEC3_UP{ 0.0f, 1.0f, 0.0f };

constexpr glm::vec3 VEC3_IDENTITY{ 0.0f };
constexpr glm::quat QUAT_IDENTITY{ 1.0f, 0.0f, 0.0f, 0.0f };
constexpr glm::mat4 MAT4_IDENTITY{ 1.0f };
constexpr glm::vec3 GRAVITY{ 0.0f, -9.8f, 0.0f };

constexpr const char* MODEL_MATRIX = "model_matrix";
constexpr const char* VIEW_MATRIX = "view_matrix";
constexpr const char* PROJECTION_MATRIX = "projection_matrix";

enum TEXTURE_UNIT {
    SKYBOX_MAP,
    DIFFUSE_MAP,
    SPECULAR_MAP,
    DEPTH_MAP_DL,
    DEPTH_MAP_ODL,
    SCENE_COLOR_IMAGE,
    SCENE_HDR_IMAGE,
    GAUSSIAN_BLURRED_IMAGE,
    PARTICLE_TEXTURE
};

#endif // CONSTANTS_H
