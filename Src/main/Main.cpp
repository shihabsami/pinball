#include "../utility/Global.h"
#include "../components/Model_Library.h"
#include "../components/Shader_Library.h"
#include "App.h"

int Global::window_width = 0;
int Global::window_height = 0;
float Global::dt = 0.0f;
float Global::dt_modded = 0.0f;
float Global::incline = 0.0f;
const Uint8* Global::key_state = nullptr;
std::vector<std::tuple<glm::vec3, glm::vec3, float>> Global::debug_lines;

std::unordered_map<const char*, std::shared_ptr<Model>> Model_Library::loaded;
std::unordered_map<Shader_Type, std::shared_ptr<Shader>> Shader_Library::shaders;

std::shared_ptr<Model> Model_Library::at(const char* path) {
    auto it = loaded.find(path);
    if (it != loaded.end()) {
        return it->second->shared_from_this();
    } else {
        loaded.emplace(path, std::make_shared<Model>(path)).first->second->init();
        return loaded.at(path)->shared_from_this();
    }
}

int main() {
    std::unique_ptr<App> app = std::make_unique<App>("Graphics Template", true);
    if (app->init(false) != APP_SUCCESS) {
        fprintf(stderr, "ERROR: Failed to initialise application.\n");
        return EXIT_FAILURE;
    }

    app->run();
    app->done();

    return EXIT_SUCCESS;
}
