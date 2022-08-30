#ifndef SCENE_H
#define SCENE_H

#include "../utility/Debug_Quad.h"
#include "../objects/Skybox.h"
#include "../components/Camera.h"
#include "../components/Shader.h"
#include "../components/Particle_System.h"
#include "../physics/Collider.h"
#include "../physics/Octree.h"
#include "../utility/Constants.h"
#include "../objects/Flipper.h"

#include <memory>
#include <array>
#include <algorithm>

class Scene {
public:
    void init();

    void display() const;

    void depth_map_pass() const;

    void hdr_pass() const;

    void gaussian_pass() const;

    void bloom_pass() const;

    void render_debug() const;

    void control_target();

    void launch_ball();

    void update();

    void done();

    std::unique_ptr<Camera> m_camera;
    std::unique_ptr<Skybox> m_skybox;
    std::unique_ptr<Debug_Quad> m_quad;
    std::shared_ptr<Lighting_Model> m_lighting_model;
    std::shared_ptr<Particle_System> m_particle_system;
    std::shared_ptr<Object> m_target;
    std::shared_ptr<Flipper> m_flipper_left;
    std::shared_ptr<Flipper> m_flipper_right;

    unsigned int m_depth_map_FBO_dl{};
    unsigned int m_depth_map_texture_dl{};
    unsigned int m_depth_map_FBO_odl{};
    unsigned int m_depth_map_texture_odl{};
    unsigned int hdr_textures[2];
    unsigned int hdr_FBO;
    unsigned int gaussian_textures[2];
    unsigned int gaussian_FBO[2];

    bool bloom_enabled{ true };
    bool debug_enabled{ false };
    int depth_map_visualiser_index{ -1 };
    int FBO_visualiser_index{ 3 };

    std::shared_ptr<Octree::Node> m_octree;
    std::vector<std::shared_ptr<Object>> m_objects;
    std::unordered_map<const char*, std::shared_ptr<Object>> m_named_objects;
};

#endif // SCENE_H
