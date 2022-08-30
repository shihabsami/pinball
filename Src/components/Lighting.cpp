#include "Lighting.h"

#include "Model.h"
#include "Model_Library.h"

void Lighting_Model::init() {
    m_visualiser_cube = Model_Library::at(CUBE_MODEL);
    m_visualiser_cube->init();
}

void Lighting_Model::render() {
    glMatrixMode(GL_MODELVIEW);
    glm::mat4 model_matrix;
    for (const auto& light: m_point_lights) {
        if (!light->visualise)
            continue;

        model_matrix = glm::translate(MAT4_IDENTITY, light->position);
        glPushMatrix();
        glMultMatrixf(glm::value_ptr(model_matrix));
        m_visualiser_cube->render();
        glPopMatrix();
    }
}

void Lighting_Model::render(std::shared_ptr<Shader>& shader) {
    shader->use();
    glm::mat4 model_matrix;
    for (const auto& light: m_point_lights) {
        if (!light->visualise)
            continue;

        model_matrix = glm::translate(MAT4_IDENTITY, light->position);
        shader->set_mat4(MODEL_MATRIX, model_matrix);
        shader->set_vec3("diffuse_color", light->diffuse);
        m_visualiser_cube->render(shader);
    }
}

void Lighting_Model::update() {
    for (size_t i = 0, remaining = m_point_lights.size(); i < remaining; i++) {
        auto& light = m_point_lights.at(i);
        if (light->temporary) {
            light->life -= Global::dt_modded;
            light->ambient *= light->life;
            light->diffuse *= light->life;
            light->specular *= light->life;

            if (light->life <= 0.0f) {
                m_point_lights.erase(m_point_lights.begin() + (int) i);
                i--;
                remaining--;
                m_light_count--;
            }
        }
    }
}

void Lighting_Model::done() {
    m_visualiser_cube->done();
    m_visualiser_cube.reset();
    m_directional_light.reset();
    for (auto& light: m_point_lights)
        light.reset();
}
