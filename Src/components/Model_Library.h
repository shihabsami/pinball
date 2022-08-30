#ifndef MODEL_LIBRARY_H
#define MODEL_LIBRARY_H

#include "Model.h"

#include <string>
#include <memory>
#include <unordered_map>

constexpr const char* CUBE_MODEL = R"(Src/assets/model/cube.obj)";
constexpr const char* WOODEN_CUBE_MODEL = R"(Src/assets/model/wooden-cube.obj)";
constexpr const char* ICO_SPHERE_MODEL = R"(Src/assets/model/ico_sphere.obj)";
constexpr const char* UV_SPHERE_MODEL = R"(Src/assets/model/uv_sphere.obj)";
constexpr const char* BV_SPHERE_MODEL = R"(Src/assets/model/bv_sphere.obj)";
constexpr const char* CORNER_BLOCK = R"(Src/assets/model/corner-block.obj)";
constexpr const char* FLIPPER = R"(Src/assets/model/flipper.obj)";
constexpr const char* CYLINDRICAL_PEG = R"(Src/assets/model/cylindrical-peg.obj)";

namespace Model_Library {
    extern std::shared_ptr<Model> at(const char* path);

    extern std::unordered_map<const char*, std::shared_ptr<Model>> loaded;
}

#endif // MODEL_LIBRARY_H
