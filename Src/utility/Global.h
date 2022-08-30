#ifndef GLOBAL_H
#define GLOBAL_H

#define SDL_MAIN_HANDLED
#include <sdl/SDL.h>

#include <glm/glm.hpp>

#include <vector>
#include <tuple>

namespace Global {
    extern int window_width;
    extern int window_height;
    extern float dt;
    extern float dt_modded;
    extern float incline;
    extern const Uint8* key_state;
    extern std::vector<std::tuple<glm::vec3, glm::vec3, float>> debug_lines;
}

#endif // GLOBAL_H
