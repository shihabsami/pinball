#ifndef LIGHTING_H
#define LIGHTING_H

#include "../utility/Constants.h"
#include "../utility/Global.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <array>
#include <vector>
#include <iostream>
#include <memory>

class Model;

class Shader;

enum Light_Space_Direction {
    RIGHT, LEFT, TOP, BOTTOM, NEAR, FAR
};

enum Light_Type {
    DIRECTIONAL, POINT
};

class Directional_Light {
public:
    explicit Directional_Light(const glm::vec3& direction)
        :
        type(DIRECTIONAL),
        ambient(glm::vec3(0.2f, 0.2f, 0.2f)),
        diffuse(glm::vec3(0.2f, 0.2f, 0.2f)),
        specular(glm::vec3(0.2f, 0.2f, 0.2f)),
        direction(direction) {
        update();
    }

    void update() {
        m_light_space_matrix = glm::mat4(
            glm::ortho(
                -DEPTH_PROJECTION_CLIP_SIZE, DEPTH_PROJECTION_CLIP_SIZE,
                -DEPTH_PROJECTION_CLIP_SIZE, DEPTH_PROJECTION_CLIP_SIZE,
                DEPTH_PROJECTION_Z_NEAR, DEPTH_PROJECTION_Z_FAR)
                *
                glm::lookAt(-direction * DEPTH_PROJECTION_CLIP_SIZE * 0.5f, VEC3_IDENTITY, VEC3_UP)
        );
    }

    Light_Type type;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 direction;
    glm::mat4 m_light_space_matrix{};
};

class Point_Light {
public:
    explicit Point_Light(
        const glm::vec3& position,
        const glm::vec3& diffuse_color,
        bool shadows,
        bool temporary,
        bool visualise)
        :
        type(POINT),
        ambient(glm::vec3(0.5f, 0.5f, 0.5f)),
        diffuse(diffuse_color),
        specular(glm::vec3(0.5f, 0.5f, 0.5f)),
        position(position),
        constant(1.0f),
        linear(0.35f),
        quadratic(0.44f),
        shadows(shadows),
        temporary(temporary),
        visualise(visualise) {
        m_projection_matrix = glm::perspective(
            glm::radians(DEPTH_PROJECTION_FOV),
            (float) SHADOW_MAP_WIDTH / (float) SHADOW_MAP_HEIGHT,
            PROJECTION_Z_NEAR,
            PROJECTION_Z_FAR
        );

        life = temporary ? 1.0f : 0.0f;
        update();
    }

    void update() {
        m_light_space_matrices.at(RIGHT) = glm::mat4(m_projection_matrix * glm::lookAt(
            position, position + VEC3_RIGHT, -VEC3_UP));
        m_light_space_matrices.at(LEFT) = glm::mat4(m_projection_matrix * glm::lookAt(
            position, position - VEC3_RIGHT, -VEC3_UP));
        m_light_space_matrices.at(TOP) = glm::mat4(m_projection_matrix * glm::lookAt(
            position, position + VEC3_UP, -VEC3_FORWARD));
        m_light_space_matrices.at(BOTTOM) = glm::mat4(m_projection_matrix * glm::lookAt(
            position, position - VEC3_UP, VEC3_FORWARD));
        m_light_space_matrices.at(NEAR) = glm::mat4(m_projection_matrix * glm::lookAt(
            position, position - VEC3_FORWARD, -VEC3_UP));
        m_light_space_matrices.at(FAR) = glm::mat4(m_projection_matrix * glm::lookAt(
            position, position + VEC3_FORWARD, -VEC3_UP));
    }

    Light_Type type;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 position;
    float constant;
    float linear;
    float quadratic;
    float life{};
    bool shadows{};
    bool temporary{};
    bool visualise{};
    glm::mat4 m_projection_matrix{};
    std::array<glm::mat4, 6> m_light_space_matrices{};
};

class Lighting_Model {
public:
    void init();

    void set_directional_light(const Directional_Light& light) {
        if (m_directional_light) {
            fprintf(stderr, "ERROR: Directional light is set already\n");
        } else if (m_light_count >= MAX_LIGHTS) {
            fprintf(stderr, "ERROR: Maximum number of lights added already\n");
        } else {
            m_directional_light = std::make_unique<Directional_Light>(light);
            m_light_count++;
        }
    }

    void add_point_light(const Point_Light& light) {
        if (m_light_count >= MAX_LIGHTS) {
            fprintf(stderr, "ERROR: Maximum number of lights added already\n");
        } else {
            m_point_lights.push_back(std::make_unique<Point_Light>(light));
            m_light_count++;
        }
    }

    void update();

    void render();

    void render(std::shared_ptr<Shader>& shader);

    void done();

    int m_light_count{};
    std::unique_ptr<Directional_Light> m_directional_light;
    std::vector<std::unique_ptr<Point_Light>> m_point_lights;
    std::shared_ptr<Model> m_visualiser_cube;
};

#endif // LIGHTING_H
