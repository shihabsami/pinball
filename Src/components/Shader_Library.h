#ifndef SHADER_LIBRARY_H
#define SHADER_LIBRARY_H

#include "Shader.h"

#include <string>
#include <unordered_map>

enum Shader_Type {
    DEFAULT,
    SPHERICAL,
    ENVIRONMENT_MAPPED,
    SKYBOX,
    DEPTH_MAP_DIRECTIONAL,
    DEPTH_MAP_OMNIDIRECTIONAL,
    HDR,
    GAUSSIAN_BLUR,
    BLOOM,
    LIGHT,
    PARTICLE,
    DEPTH_DEBUG
};

namespace Shader_Library {
    extern std::unordered_map<Shader_Type, std::shared_ptr<Shader>> shaders;
}

#endif // SHADER_LIBRARY_H
