#ifndef APP_H
#define APP_H

#include "Scene.h"
#include "../utility/Constants.h"

#include <glad/glad.h>
#include <sdl/SDL.h>

#define GLT_IMPLEMENTATION
#include <gltext/gltext.h>

#include <iostream>
#include <array>
#include <unordered_map>
#include <memory>

#define APP_SUCCESS EXIT_SUCCESS
#define APP_FAILURE EXIT_FAILURE

class App {
public:
    explicit App(const char* title, bool fullscreen = false, int width = SCREEN_WIDTH, int height = SCREEN_HEIGHT);

    int init(bool debug);

    void run();

    void update();

    void done();

private:
    void render();

    void render_osd() const;

    void handle_input();

    bool is_available(SDL_Scancode key);

    const char* m_window_title;
    bool m_fullscreen;
    int m_window_width;
    int m_window_height;
    int m_screen_width{};
    int m_screen_height{};
    SDL_Window* m_sdl_window{};
    SDL_GLContext m_gl_context{};
    std::unique_ptr<Scene> m_scene;

    bool m_quit{};
    bool m_first_frame{ true };
    float m_framerate{};
    unsigned int m_elapsed_frames{};
    bool paused{ false };
    float playback_speed{ 1.0f };
    std::unordered_map<SDL_Scancode, float> m_key_timers;
};

#endif // APP_H
