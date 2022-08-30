#include "Scene.h"

#include "../components/Model_Library.h"
#include "../components/Shader_Library.h"
#include "../utility/Debug.h"
#include "../objects/Flipper.h"

void Scene::init() {
    m_lighting_model = std::make_shared<Lighting_Model>();
    m_lighting_model->init();
    m_lighting_model->set_directional_light(Directional_Light{ glm::normalize(glm::vec3(0.5f, -0.5f, -1.0f)) });
    m_lighting_model->add_point_light(Point_Light{
        glm::vec3{ 0.0f, 7.5f, 4.0f },
        glm::vec3(0.0f, 2.5f, 5.0f), true, false, true });

    m_particle_system = std::make_shared<Particle_System>();
    m_particle_system->init(m_lighting_model);

    m_camera = std::make_unique<Camera>(
        PROJECTION_FOV,
        (float) Global::window_width / (float) Global::window_height,
        PROJECTION_Z_NEAR,
        PROJECTION_Z_FAR
    );
    m_camera->translate(glm::vec3{ 0.0f, 0.0f, 75.0f });

    // for visualising depth map
    m_quad = std::make_unique<Debug_Quad>();
    m_quad->init();

    m_skybox = std::make_unique<Skybox>(std::vector<const char*>(
        {
            R"(Src/assets/skybox/px.png)",
            R"(Src/assets/skybox/nx.png)",
            R"(Src/assets/skybox/py.png)",
            R"(Src/assets/skybox/ny.png)",
            R"(Src/assets/skybox/pz.png)",
            R"(Src/assets/skybox/nz.png)"
        }));
    m_skybox->init();

    auto shader_default =
        Shader_Library::shaders.emplace(DEFAULT, std::make_shared<Shader>()).first->second;
    shader_default->init(
        R"(Src/shaders/default/Vertex.glsl)",
        R"(Src/shaders/default/Fragment.glsl)",
        R"(Src/shaders/default/Geometry.glsl)");
    shader_default->use();
    shader_default->set_vec3("view_position", m_camera->m_position);
    shader_default->set_mat4(VIEW_MATRIX, m_camera->m_view_matrix);
    shader_default->set_mat4(PROJECTION_MATRIX, m_camera->m_projection_matrix);
    if (m_lighting_model->m_directional_light)
        shader_default->set_mat4("light_space_matrix", m_lighting_model->m_directional_light->m_light_space_matrix);
    shader_default->set_float("far_plane", PROJECTION_Z_FAR);
    shader_default->set_int("depth_map_dl", DEPTH_MAP_DL);
    shader_default->set_int("depth_map_odl", DEPTH_MAP_ODL);
    shader_default->set_lighting_model(*m_lighting_model);

    auto shader_spherical =
        Shader_Library::shaders.emplace(SPHERICAL, std::make_shared<Shader>()).first->second;
    shader_spherical->init(
        R"(Src/shaders/default/Vertex.glsl)",
        R"(Src/shaders/default/Fragment.glsl)",
        R"(Src/shaders/sphere/Geometry.glsl)");
    shader_spherical->use();
    shader_spherical->set_vec3("view_position", m_camera->m_position);
    shader_spherical->set_mat4(VIEW_MATRIX, m_camera->m_view_matrix);
    shader_spherical->set_mat4(PROJECTION_MATRIX, m_camera->m_projection_matrix);
    if (m_lighting_model->m_directional_light)
        shader_spherical->set_mat4("light_space_matrix", m_lighting_model->m_directional_light->m_light_space_matrix);
    shader_spherical->set_float("far_plane", PROJECTION_Z_FAR);
    shader_spherical->set_int("depth_map_dl", DEPTH_MAP_DL);
    shader_spherical->set_int("depth_map_odl", DEPTH_MAP_ODL);
    shader_spherical->set_lighting_model(*m_lighting_model);

    auto shader_env_map =
        Shader_Library::shaders.emplace(ENVIRONMENT_MAPPED, std::make_shared<Shader>()).first->second;
    shader_env_map->init(
        R"(Src/shaders/default/Vertex.glsl)",
        R"(Src/shaders/environment-mapped/Fragment.glsl)",
        R"(Src/shaders/default/Geometry.glsl)");
    shader_env_map->use();
    shader_env_map->set_mat4(VIEW_MATRIX, m_camera->m_view_matrix);
    shader_env_map->set_mat4(PROJECTION_MATRIX, m_camera->m_projection_matrix);
    if (m_lighting_model->m_directional_light)
        shader_env_map->set_mat4("light_space_matrix", m_lighting_model->m_directional_light->m_light_space_matrix);

    auto shader_skybox =
        Shader_Library::shaders.emplace(SKYBOX, std::make_shared<Shader>()).first->second;
    shader_skybox->init(
        R"(Src/shaders/skybox/Vertex.glsl)",
        R"(Src/shaders/skybox/Fragment.glsl)");
    shader_skybox->use();
    shader_skybox->set_mat4(VIEW_MATRIX, glm::mat4(glm::mat3(m_camera->m_view_matrix)));
    shader_skybox->set_mat4(PROJECTION_MATRIX, m_camera->m_projection_matrix);
    shader_skybox->set_int("skybox_map", SKYBOX_MAP);

    auto shader_depth_map_dl =
        Shader_Library::shaders.emplace(DEPTH_MAP_DIRECTIONAL, std::make_shared<Shader>()).first->second;
    shader_depth_map_dl->init(
        R"(Src/shaders/depth-map-directional/Vertex.glsl)",
        R"(Src/shaders/depth-map-directional/Fragment.glsl)");

    auto shader_depth_map_odl =
        Shader_Library::shaders.emplace(DEPTH_MAP_OMNIDIRECTIONAL, std::make_shared<Shader>()).first->second;
    shader_depth_map_odl->init(
        R"(Src/shaders/depth-map-omnidirectional/Vertex.glsl)",
        R"(Src/shaders/depth-map-omnidirectional/Fragment.glsl)",
        R"(Src/shaders/depth-map-omnidirectional/Geometry.glsl)");
    shader_depth_map_odl->use();
    shader_depth_map_odl->set_float("far_plane", PROJECTION_Z_FAR);

    auto shader_hdr =
        Shader_Library::shaders.emplace(HDR, std::make_shared<Shader>()).first->second;
    shader_hdr->init(
        R"(Src/shaders/default/Vertex.glsl)",
        R"(Src/shaders/default/Fragment.glsl)",
        R"(Src/shaders/default/Geometry.glsl)");
    shader_hdr->use();
    shader_hdr->set_vec3("view_position", m_camera->m_position);
    shader_hdr->set_mat4(VIEW_MATRIX, m_camera->m_view_matrix);
    shader_hdr->set_mat4(PROJECTION_MATRIX, m_camera->m_projection_matrix);
    shader_hdr->set_lighting_model(*m_lighting_model);

    auto shader_gaussian =
        Shader_Library::shaders.emplace(GAUSSIAN_BLUR, std::make_shared<Shader>()).first->second;
    shader_gaussian->init(
        R"(Src/shaders/gaussian-blur/Vertex.glsl)",
        R"(Src/shaders/gaussian-blur/Fragment.glsl)");
    shader_gaussian->use();
    shader_gaussian->set_int("image", SCENE_COLOR_IMAGE);

    auto shader_bloom =
        Shader_Library::shaders.emplace(BLOOM, std::make_shared<Shader>()).first->second;
    shader_bloom->init(
        R"(Src/shaders/gaussian-blur/Vertex.glsl)",
        R"(Src/shaders/bloom/Fragment.glsl)");
    shader_bloom->use();
    shader_bloom->set_int("color_image", SCENE_COLOR_IMAGE);
    shader_bloom->set_int("hdr_image", SCENE_HDR_IMAGE);
    shader_bloom->set_int("blurred_image", GAUSSIAN_BLURRED_IMAGE);

    auto shader_depth_debug =
        Shader_Library::shaders.emplace(DEPTH_DEBUG, std::make_shared<Shader>()).first->second;
    shader_depth_debug->init(
        R"(Src/shaders/depth-visualiser/Vertex.glsl)",
        R"(Src/shaders/depth-visualiser/Fragment.glsl)");
    shader_depth_debug->use();
    shader_depth_debug->set_int("depth_map_dl", DEPTH_MAP_DL);
    shader_depth_debug->set_mat4(MODEL_MATRIX, MAT4_IDENTITY);

    auto shader_light =
        Shader_Library::shaders.emplace(LIGHT, std::make_shared<Shader>()).first->second;
    shader_light->init(
        R"(Src/shaders/light-visualiser/Vertex.glsl)",
        R"(Src/shaders/light-visualiser/Fragment.glsl)");
    shader_light->use();
    shader_light->set_mat4(VIEW_MATRIX, m_camera->m_view_matrix);
    shader_light->set_mat4(PROJECTION_MATRIX, m_camera->m_projection_matrix);

    auto shader_particle =
        Shader_Library::shaders.emplace(PARTICLE, std::make_shared<Shader>()).first->second;
    shader_particle->init(
        R"(Src/shaders/particle/Vertex.glsl)",
        R"(Src/shaders/particle/Fragment.glsl)",
        R"(Src/shaders/particle/Geometry.glsl)");
    shader_particle->use();
    shader_particle->set_vec3("camera_up", m_camera->m_up);
    shader_particle->set_vec3("camera_right", m_camera->m_right);
    shader_particle->set_mat4(VIEW_MATRIX, m_camera->m_view_matrix);
    shader_particle->set_mat4(PROJECTION_MATRIX, m_camera->m_projection_matrix);
    shader_particle->set_int("particle_texture", PARTICLE_TEXTURE);

    // Generating the directional depth map texture
    glGenTextures(1, &m_depth_map_texture_dl);
    glBindTexture(GL_TEXTURE_2D, m_depth_map_texture_dl);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR,
        glm::value_ptr(glm::vec4{ 1.0, 1.0, 1.0, 1.0 }));
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    // Generating the directional depth map framebuffer object
    glGenFramebuffers(1, &m_depth_map_FBO_dl);
    glBindFramebuffer(GL_FRAMEBUFFER, m_depth_map_FBO_dl);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth_map_texture_dl, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        fprintf(stderr, "ERROR: Incomplete directional light framebuffer\n");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Generating the omnidirectional depth map texture
    glGenTextures(1, &m_depth_map_texture_odl);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_depth_map_texture_odl);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    for (unsigned int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
            SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    // Generating the omnidirectional depth map framebuffer object
    glGenFramebuffers(1, &m_depth_map_FBO_odl);
    glBindFramebuffer(GL_FRAMEBUFFER, m_depth_map_FBO_odl);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depth_map_texture_odl, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        fprintf(stderr, "ERROR: Incomplete omnidirectional light framebuffer\n");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Generating the hdr framebuffer object and the two textures as color attachments
    glGenFramebuffers(1, &hdr_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, hdr_FBO);
    glGenTextures(2, hdr_textures);
    for (unsigned int i = 0; i < 2; i++) {
        glBindTexture(GL_TEXTURE_2D, hdr_textures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Global::window_width, Global::window_height, 0, GL_RGBA, GL_FLOAT,
            nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, hdr_textures[i], 0);
    }

    // Generating the renderbuffer object to quickly switch between color attachments
    unsigned int depth_RBO;
    glGenRenderbuffers(1, &depth_RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, Global::window_width, Global::window_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_RBO);
    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        fprintf(stderr, "ERROR: Incomplete HDR framebuffer\n");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Generating the gaussian framebuffer object for gaussian blur post-processing effect
    glGenFramebuffers(2, gaussian_FBO);
    glGenTextures(2, gaussian_textures);
    for (unsigned int i = 0; i < 2; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, gaussian_FBO[i]);
        glBindTexture(GL_TEXTURE_2D, gaussian_textures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Global::window_width, Global::window_height, 0, GL_RGBA, GL_FLOAT,
            nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gaussian_textures[i], 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            fprintf(stderr, "ERROR: Incomplete Gaussian Blur framebuffer\n");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glLoadMatrixf(glm::value_ptr(m_camera->m_projection_matrix));

    m_named_objects.emplace(
        "table_left",
        std::make_shared<Object>(
            Model_Library::at(WOODEN_CUBE_MODEL),
            glm::vec3{ -16.125f, 0.0f, 0.0f },
            QUAT_IDENTITY,
            glm::vec3(2.5f, 52.5f, 10.0f),
            std::numeric_limits<float>::infinity(),
            Bound_Type::AABB)
    ).first->second->init();

    m_named_objects.emplace(
        "table_right",
        std::make_shared<Object>(
            Model_Library::at(WOODEN_CUBE_MODEL),
            glm::vec3{ 16.125f, 0.0f, 0.0f },
            QUAT_IDENTITY,
            glm::vec3(2.5f, 52.5f, 10.0f),
            std::numeric_limits<float>::infinity(),
            Bound_Type::AABB)
    ).first->second->init();

    m_named_objects.emplace(
        "table_top",
        std::make_shared<Object>(
            Model_Library::at(WOODEN_CUBE_MODEL),
            glm::vec3{ 0.0f, 25.0f, 0.0f },
            QUAT_IDENTITY,
            glm::vec3(30.0f, 2.5f, 10.0f),
            std::numeric_limits<float>::infinity(),
            Bound_Type::AABB)
    ).first->second->init();

    m_named_objects.emplace(
        "table_bottom",
        std::make_shared<Object>(
            Model_Library::at(WOODEN_CUBE_MODEL),
            glm::vec3{ 13.1f, -25.0f, 0.0f },
            QUAT_IDENTITY,
            glm::vec3(4.5f, 2.5f, 10.0f),
            1.0f,
            Bound_Type::AABB)
    ).first->second->init();

    m_named_objects.emplace(
        "table_back",
        std::make_shared<Object>(
            Model_Library::at(WOODEN_CUBE_MODEL),
            glm::vec3{ 0.0f, 0.0f, -5.5f },
            QUAT_IDENTITY,
            glm::vec3(34.75f, 52.5f, 1.0f),
            std::numeric_limits<float>::infinity(),
            Bound_Type::AABB)
    ).first->second->init();

    m_named_objects.emplace(
        "table_front",
        std::make_shared<Object>(
            Model_Library::at(WOODEN_CUBE_MODEL),
            glm::vec3{ 0.0f, 0.0f, 5.5f },
            QUAT_IDENTITY,
            glm::vec3(34.75f, 52.5f, 1.0f),
            std::numeric_limits<float>::infinity(),
            Bound_Type::AABB)
    ).first->second->init();
    m_named_objects.at("table_front")->m_render_enabled = false;

    m_named_objects.emplace(
        "launch_path_wall",
        std::make_shared<Object>(
            Model_Library::at(WOODEN_CUBE_MODEL),
            glm::vec3{ 11.5f, -2.5f, 0.0f },
            QUAT_IDENTITY,
            glm::vec3(1.25f, 42.5f, 10.0f),
            std::numeric_limits<float>::infinity(),
            Bound_Type::AABB)
    ).first->second->init();

    m_named_objects.emplace(
        "launch_path_corner",
        std::make_shared<Object>(
            Model_Library::at(CORNER_BLOCK),
            glm::vec3{ 13.35f, 22.20f, -0.05f },
            QUAT_IDENTITY,
            glm::vec3(8.0f, 8.0f, 10.0f),
            std::numeric_limits<float>::infinity(),
            Bound_Type::AABB)
    ).first->second->init();

    m_named_objects.emplace(
        "cylindrical_peg1",
        std::make_shared<Object>(
            Model_Library::at(CYLINDRICAL_PEG),
            glm::vec3{ -7.0f, 14.5f, 0.0f },
            QUAT_IDENTITY,
            glm::vec3(2.5f, 2.5f, 10.0f),
            std::numeric_limits<float>::infinity(),
            Bound_Type::AABB)
    ).first->second->init();

    m_named_objects.emplace(
        "cylindrical_peg2",
        std::make_shared<Object>(
            Model_Library::at(CYLINDRICAL_PEG),
            glm::vec3{ 7.0f, 14.5f, 0.0f },
            QUAT_IDENTITY,
            glm::vec3(2.5f, 2.5f, 10.0f),
            std::numeric_limits<float>::infinity(),
            Bound_Type::AABB)
    ).first->second->init();

    m_named_objects.emplace(
        "cylindrical_peg3",
        std::make_shared<Object>(
            Model_Library::at(CYLINDRICAL_PEG),
            glm::vec3{ 0.0f, 7.5f, 0.0f },
            QUAT_IDENTITY,
            glm::vec3(2.5f, 2.5f, 10.0f),
            std::numeric_limits<float>::infinity(),
            Bound_Type::AABB)
    ).first->second->init();

    m_named_objects.emplace(
        "cylindrical_peg4",
        std::make_shared<Object>(
            Model_Library::at(CYLINDRICAL_PEG),
            glm::vec3{ -7.0f, 0.5f, 0.0f },
            QUAT_IDENTITY,
            glm::vec3(2.5f, 2.5f, 10.0f),
            std::numeric_limits<float>::infinity(),
            Bound_Type::AABB)
    ).first->second->init();

    m_named_objects.emplace(
        "cylindrical_peg5",
        std::make_shared<Object>(
            Model_Library::at(CYLINDRICAL_PEG),
            glm::vec3{ 7.0f, 0.5f, 0.0f },
            QUAT_IDENTITY,
            glm::vec3(2.5f, 2.5f, 10.0f),
            std::numeric_limits<float>::infinity(),
            Bound_Type::AABB)
    ).first->second->init();

    for (auto& object: m_named_objects)
        m_objects.push_back(object.second);

    Global::incline = 0.0f;

    m_flipper_right = std::make_shared<Flipper>(
        Model_Library::at(FLIPPER),
        glm::vec3{ 18.0f, -21.0f, -0.1f },
        QUAT_IDENTITY,
        glm::vec3(12.5f, 1.0f, 10.0f),
        std::numeric_limits<float>::infinity(),
        Bound_Type::AABB
    );
    m_flipper_right->init(165.0f, 210.0f, glm::vec3{ 11.5, -20.5, -0.1 }, false);
    m_objects.push_back(m_flipper_right);

    m_flipper_left = std::make_shared<Flipper>(
        Model_Library::at(FLIPPER),
        glm::vec3{ -9.0f, -20.0f, -0.1f },
        QUAT_IDENTITY,
        glm::vec3(12.5f, 1.0f, 10.0f),
        std::numeric_limits<float>::infinity(),
        Bound_Type::AABB
    );
    m_flipper_left->init(-15.0f, 30.0f, glm::vec3{ -15.5, -20.5, -0.1 }, true);
    m_objects.push_back(m_flipper_left);

    m_named_objects.at("cylindrical_peg1")->m_rb->has_outward_force = true;
    m_named_objects.at("cylindrical_peg2")->m_rb->has_outward_force = true;
    m_named_objects.at("cylindrical_peg3")->m_rb->has_outward_force = true;
    m_named_objects.at("cylindrical_peg4")->m_rb->has_outward_force = true;
    m_named_objects.at("cylindrical_peg5")->m_rb->has_outward_force = true;

    m_octree = std::make_shared<Octree::Node>(
        Bounding_Region(
            glm::vec3(-30.0f, -40.0f, -30.0f),
            glm::vec3(30.0f, 30.0f, 30.0f)),
        m_objects);
    m_octree->build();
}

void Scene::depth_map_pass() const {
    glViewport(0, 0, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);
    if (m_lighting_model->m_directional_light) {
        glBindFramebuffer(GL_FRAMEBUFFER, m_depth_map_FBO_dl);
        glClear(GL_DEPTH_BUFFER_BIT);
        auto shader = Shader_Library::shaders.at(DEPTH_MAP_DIRECTIONAL);
        shader->use();
        shader->set_mat4("light_space_matrix",
            m_lighting_model->m_directional_light->m_light_space_matrix);

        m_octree->render(shader);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    if (!m_lighting_model->m_point_lights.empty()) {
        glBindFramebuffer(GL_FRAMEBUFFER, m_depth_map_FBO_odl);
        glClear(GL_DEPTH_BUFFER_BIT);
        auto shader = Shader_Library::shaders.at(DEPTH_MAP_OMNIDIRECTIONAL);
        shader->use();
        for (const auto& light: m_lighting_model->m_point_lights) {
            if (!light->shadows)
                continue;

            shader->set_vec3("light_position", light->position);
            for (int i = 0; i < 6; ++i)
                shader->set_array_mat4("light_space_matrices", i, light->m_light_space_matrices.at(i));

            m_octree->render(shader);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void Scene::hdr_pass() const {
    glViewport(0, 0, Global::window_width, Global::window_height);
    glBindFramebuffer(GL_FRAMEBUFFER, hdr_FBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0 + DEPTH_MAP_DL);
    glBindTexture(GL_TEXTURE_2D, m_depth_map_texture_dl);
    glActiveTexture(GL_TEXTURE0 + DEPTH_MAP_ODL);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_depth_map_texture_odl);

    if (depth_map_visualiser_index == DIRECTIONAL) {
        // For visualising depth map
        auto shader_depth = Shader_Library::shaders.at(DEPTH_DEBUG);
        shader_depth->use();
        m_quad->render();
    }

    auto shader = Shader_Library::shaders.at(ENVIRONMENT_MAPPED);
    shader->use();
    shader->set_vec3("view_position", m_camera->m_position);
    shader->set_mat4("view_matrix", m_camera->m_view_matrix);
    shader->set_int("depth_map_visualiser_index", depth_map_visualiser_index);
    shader->set_lighting_model(*m_lighting_model);

    shader = Shader_Library::shaders.at(DEFAULT);
    shader->use();
    shader->set_vec3("view_position", m_camera->m_position);
    shader->set_mat4("view_matrix", m_camera->m_view_matrix);
    shader->set_int("depth_map_visualiser_index", depth_map_visualiser_index);
    shader->set_lighting_model(*m_lighting_model);
    m_octree->render(shader);

    shader = Shader_Library::shaders.at(PARTICLE);
    shader->use();
    shader->set_vec3("camera_right", m_camera->m_right);
    shader->set_vec3("camera_up", m_camera->m_up);
    shader->set_mat4("view_matrix", m_camera->m_view_matrix);
    glActiveTexture(GL_TEXTURE0 + PARTICLE_TEXTURE);
    glBindTexture(GL_TEXTURE_2D, m_particle_system->texture);
    m_particle_system->render();

    shader = Shader_Library::shaders.at(LIGHT);
    shader->use();
    shader->set_mat4("view_matrix", m_camera->m_view_matrix);
    m_lighting_model->render(shader);

    shader = Shader_Library::shaders.at(SKYBOX);
    shader->use();
    shader->set_mat4("view_matrix", glm::mat4(glm::mat3(m_camera->m_view_matrix)));
    m_skybox->render(shader);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::gaussian_pass() const {
    bool horizontal = true, first_iteration = true;
    unsigned int amount = 10;
    auto& shader = Shader_Library::shaders.at(GAUSSIAN_BLUR);
    shader->use();
    for (unsigned int i = 0; i < amount; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, gaussian_FBO[horizontal]);
        shader->set_bool("horizontal", horizontal);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, first_iteration ? hdr_textures[1] : gaussian_textures[!horizontal]);
        m_quad->render();

        horizontal = !horizontal;
        if (first_iteration)
            first_iteration = false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::bloom_pass() const {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    auto shader = Shader_Library::shaders.at(BLOOM);
    shader->use();
    shader->set_int("FBO_debug_index", FBO_visualiser_index);
    shader->set_bool("bloom", bloom_enabled);
    shader->set_float("exposure", 0.5f);

    glActiveTexture(GL_TEXTURE0 + SCENE_COLOR_IMAGE);
    glBindTexture(GL_TEXTURE_2D, hdr_textures[0]);
    glActiveTexture(GL_TEXTURE0 + SCENE_HDR_IMAGE);
    glBindTexture(GL_TEXTURE_2D, hdr_textures[1]);
    glActiveTexture(GL_TEXTURE0 + GAUSSIAN_BLURRED_IMAGE);
    glBindTexture(GL_TEXTURE_2D, gaussian_textures[1]);
    m_quad->render();
}

void Scene::render_debug() const {
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glLoadMatrixf(glm::value_ptr(m_camera->m_view_matrix));

    m_octree->render();
}

void Scene::display() const {
    if (debug_enabled) {
        render_debug();
    } else {
        depth_map_pass();
        hdr_pass();
        gaussian_pass();
        bloom_pass();
    }
}

void Scene::control_target() {
    // Debug method to control a target object
    float translate_factor = 10.0f;
    float rotation_factor = 90.0f;
    float scale_factor = 10.0f;

    if (Global::key_state[SDL_SCANCODE_END]) {
        print_vec3(m_target->m_rb->position, "position");
        print_quat(m_target->m_rb->rotation, "rotation");
        print_vec3(m_target->m_rb->scale, "scale");
    }

    if (Global::key_state[SDL_SCANCODE_HOME]) {
        print_vec3(m_camera->m_position, "position");
        print_quat(m_camera->m_orientation, "orientation");
    }

    if (Global::key_state[SDL_SCANCODE_KP_5])
        m_target->m_rb->position += VEC3_FORWARD * Global::dt * translate_factor;
    if (Global::key_state[SDL_SCANCODE_KP_8])
        m_target->m_rb->position -= VEC3_FORWARD * Global::dt * translate_factor;
    if (Global::key_state[SDL_SCANCODE_KP_4])
        m_target->m_rb->position -= VEC3_RIGHT * Global::dt * translate_factor;
    if (Global::key_state[SDL_SCANCODE_KP_6])
        m_target->m_rb->position += VEC3_RIGHT * Global::dt * translate_factor;
    if (Global::key_state[SDL_SCANCODE_KP_7])
        m_target->m_rb->position += VEC3_UP * Global::dt * translate_factor;
    if (Global::key_state[SDL_SCANCODE_KP_9])
        m_target->m_rb->position -= VEC3_UP * Global::dt * translate_factor;

//    if (Global::key_state[SDL_SCANCODE_I])
//        m_target->m_rb->rotation *= glm::angleAxis(glm::radians(rotation_factor * Global::dt), VEC3_FORWARD);
//    if (Global::key_state[SDL_SCANCODE_K])
//        m_target->m_rb->rotation *= glm::angleAxis(glm::radians(rotation_factor * Global::dt), -VEC3_FORWARD);
//    if (Global::key_state[SDL_SCANCODE_J])
//        m_target->m_rb->rotation *= glm::angleAxis(glm::radians(rotation_factor * Global::dt), VEC3_RIGHT);
//    if (Global::key_state[SDL_SCANCODE_L])
//        m_target->m_rb->rotation *= glm::angleAxis(glm::radians(rotation_factor * Global::dt), -VEC3_RIGHT);
//    if (Global::key_state[SDL_SCANCODE_U])
//        m_target->m_rb->rotation *= glm::angleAxis(glm::radians(rotation_factor * Global::dt), VEC3_UP);
//    if (Global::key_state[SDL_SCANCODE_O])
//        m_target->m_rb->rotation *= glm::angleAxis(glm::radians(rotation_factor * Global::dt), -VEC3_UP);

    if (Global::key_state[SDL_SCANCODE_I])
        m_target->m_rb->scale += VEC3_FORWARD * Global::dt * scale_factor;
    if (Global::key_state[SDL_SCANCODE_K])
        m_target->m_rb->scale -= VEC3_FORWARD * Global::dt * scale_factor;
    if (Global::key_state[SDL_SCANCODE_J])
        m_target->m_rb->scale -= VEC3_RIGHT * Global::dt * scale_factor;
    if (Global::key_state[SDL_SCANCODE_L])
        m_target->m_rb->scale += VEC3_RIGHT * Global::dt * scale_factor;
    if (Global::key_state[SDL_SCANCODE_U])
        m_target->m_rb->scale += VEC3_UP * Global::dt * scale_factor;
    if (Global::key_state[SDL_SCANCODE_O])
        m_target->m_rb->scale -= VEC3_UP * Global::dt * scale_factor;
}

void Scene::update() {
//    if (Global::key_state[SDL_SCANCODE_HOME]) {
//        Global::incline = Global::incline < 90.0f ? Global::incline + 90.0f * Global::dt : Global::incline;
//        Global::incline = Global::incline > 90.0f ? 90.0f : Global::incline;
//    }
//    if (Global::key_state[SDL_SCANCODE_END]) {
//        Global::incline = Global::incline > -90.0f ? Global::incline - 90.0f * Global::dt : Global::incline;
//        Global::incline = Global::incline < -90.0f ? -90.0f : Global::incline;
//    }

    m_lighting_model->update();
    m_octree->mark_dead_objects(m_octree, m_octree->m_region);
    m_octree->update(m_particle_system);
    m_particle_system->update(m_octree);
}

void Scene::launch_ball() {
    std::shared_ptr<Object> ball = std::make_shared<Object>(
        Model_Library::at(UV_SPHERE_MODEL),
        glm::vec3{ 13.5f, -20.0f, 0.0f },
        QUAT_IDENTITY,
        glm::vec3(1.0f),
        1.0f,
        Bound_Type::SPHERE_BV);
    ball->init();
    ball->m_rb->transfer_energy(500.0f, ball->m_rb->rotation * VEC3_UP);
    ball->m_rb->add_acceleration(GRAVITY);
    m_octree->insert(ball);
}

void Scene::done() {
    m_octree->destroy();
    m_skybox->done();
    m_lighting_model->done();
    m_particle_system->done();

    m_camera.reset();
    m_skybox.reset();
    m_lighting_model.reset();
    m_particle_system.reset();
    m_quad.reset();
    m_target.reset();

    glDeleteTextures(1, &m_depth_map_texture_dl);
    glDeleteFramebuffers(1, &m_depth_map_FBO_dl);
    glDeleteTextures(1, &m_depth_map_texture_odl);
    glDeleteFramebuffers(1, &m_depth_map_FBO_odl);
    glDeleteTextures(1, &m_depth_map_texture_dl);
    glDeleteFramebuffers(1, &m_depth_map_FBO_dl);
    glDeleteTextures(2, hdr_textures);
    glDeleteFramebuffers(1, &hdr_FBO);
    glDeleteTextures(2, gaussian_textures);
    glDeleteFramebuffers(2, gaussian_FBO);

    for (auto& o: m_objects)
        o.reset();

    for (auto& no: m_named_objects)
        no.second.reset();
}
