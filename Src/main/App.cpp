#include "App.h"

#include "../utility/Debug.h"
#include "../components/Model_Library.h"
#include "../components/Shader_Library.h"

App::App(const char* title, bool fullscreen, int width, int height)
    : m_window_title(title), m_fullscreen(fullscreen), m_window_width(width), m_window_height(height) {}

int App::init(bool debug) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "ERROR: SDL2 video subsystem couldn't be initialized: %s\n", SDL_GetError());
        return APP_FAILURE;
    }

    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    if (m_fullscreen) {
        m_sdl_window = SDL_CreateWindow(
            m_window_title,
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            0, 0,
            SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN);
    } else {
        m_sdl_window = SDL_CreateWindow(
            m_window_title,
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            m_window_width, m_window_height,
            SDL_WINDOW_OPENGL);
    }

    m_gl_context = SDL_GL_CreateContext(m_sdl_window);

    if (!gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress)) {
        fprintf(stderr, "ERROR: Failed to initialize the OpenGL context.\n");
        SDL_GL_DeleteContext(m_gl_context);
        SDL_DestroyWindow(m_sdl_window);
        SDL_Quit();
        return APP_FAILURE;
    }

    //  Uncomment to receive debug info
    //if (debug) {
    //    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    //    glDebugMessageCallback(gl_debug_message_callback, nullptr);
    //}

    SDL_DisplayMode dm;
    if (SDL_GetCurrentDisplayMode(0, &dm) == 0) {
        m_screen_width = dm.w;
        m_screen_height = dm.h;
        if (m_fullscreen) {
            m_window_width = m_screen_width;
            m_window_height = m_screen_height;
        }
    } else {
        printf("WARNING: SDL couldn't retrieve current display mode details.\n");
    }

    gltInit();
    printf("MESSAGE: OpenGL version %d.%d initialised.\n", GLVersion.major, GLVersion.minor);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    Global::window_height = m_window_height;
    Global::window_width = m_window_width;
    Global::key_state = SDL_GetKeyboardState(nullptr);

    m_scene = std::make_unique<Scene>();
    m_scene->init();

    return APP_SUCCESS;
}

void App::run() {
    while (!m_quit) {
        Uint64 start_time = SDL_GetPerformanceCounter();
        static Uint32 last_time = 0;
        Uint32 current_time = SDL_GetTicks();
        Uint32 dt_milli = current_time - last_time;
        last_time = current_time;
        Global::dt = (float) dt_milli * 0.001f;
        Global::dt_modded = Global::dt * playback_speed;

        SDL_PumpEvents();
        handle_input();
        update();
        render();

        Uint64 end_time = SDL_GetPerformanceCounter();
        if (m_elapsed_frames == FRAME_RATE_INTERVAL)
            m_framerate = 1.0f / ((float) (end_time - start_time) / (float) SDL_GetPerformanceFrequency());

        m_elapsed_frames = (m_elapsed_frames + 1) % (FRAME_RATE_INTERVAL + 1);
    }
}

void App::render() {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, Global::window_width, Global::window_height);

    m_scene->display();

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glViewport(0, 0, Global::window_width, Global::window_height);
    render_osd();

    SDL_GL_SwapWindow(m_sdl_window);
}

void App::render_osd() const {
    glUseProgram(0);
    GLfloat x = 10.0f;
    GLfloat y = 10.0f;
    GLfloat offset = 40.0f;

    GLTtext* fps = gltCreateText();
    gltSetText(fps, ("FPS: " + std::to_string((unsigned int) m_framerate)).c_str());
    gltBeginDraw();
    gltColor(1.0f, 1.0f, 1.0f, 1.0f);
    gltDrawText2D(fps, x, y, 2.0);
    gltEndDraw();
    gltDeleteText(fps);
    y += offset;

    SDL_DisplayMode dm;
    SDL_GetDesktopDisplayMode(0, &dm);

    GLTtext* resolution = gltCreateText();
    gltSetText(resolution, ("Resolution: " + std::to_string(dm.w) + "x" + std::to_string(dm.h)).c_str());
    gltBeginDraw();
    gltColor(1.0f, 1.0f, 1.0f, 1.0f);
    gltDrawText2D(resolution, x, y, 2.0);
    gltEndDraw();
    gltDeleteText(resolution);
    y += offset;

    GLTtext* refresh_rate = gltCreateText();
    gltSetText(refresh_rate, ("Refresh Rate: " + std::to_string(dm.refresh_rate) + " (hz)").c_str());
    gltBeginDraw();
    gltColor(1.0f, 1.0f, 1.0f, 1.0f);
    gltDrawText2D(refresh_rate, x, y, 2.0);
    gltEndDraw();
    gltDeleteText(refresh_rate);

    glUseProgram(0);
}

void App::handle_input() {
    std::unique_ptr<Camera>& camera = m_scene->m_camera;
    int rel_x = 0, rel_y = 0;
    SDL_GetRelativeMouseState(&rel_x, &rel_y);
    if (m_first_frame) {
        m_first_frame = false;
    } else {
        if (rel_y != 0) {
            camera->pitch((float) -rel_y * MOUSE_SENSITIVITY * CAMERA_ROTATION_FACTOR);
        }
        if (rel_x != 0) {
            camera->yaw((float) -rel_x * MOUSE_SENSITIVITY * CAMERA_ROTATION_FACTOR);
        }
    }

    if (Global::key_state[SDL_SCANCODE_ESCAPE])
        m_quit = true;
    const Uint8* keys;
    keys = SDL_GetKeyboardState(nullptr);
    if (keys) {
        if (keys[SDL_SCANCODE_ESCAPE])
            m_quit = true;

        if (keys[SDL_SCANCODE_W])
            camera->translate(camera->m_position + camera->m_forward * (CAMERA_MOVE_FACTOR * Global::dt));
        if (keys[SDL_SCANCODE_S])
            camera->translate(camera->m_position - camera->m_forward * (CAMERA_MOVE_FACTOR * Global::dt));
        if (keys[SDL_SCANCODE_A])
            camera->translate(camera->m_position - camera->m_right * (CAMERA_MOVE_FACTOR * Global::dt));
        if (keys[SDL_SCANCODE_D])
            camera->translate(camera->m_position + camera->m_right * (CAMERA_MOVE_FACTOR * Global::dt));
        if (keys[SDL_SCANCODE_Q])
            camera->translate(camera->m_position - camera->m_up * (CAMERA_MOVE_FACTOR * Global::dt));
        if (keys[SDL_SCANCODE_E])
            camera->translate(camera->m_position + camera->m_up * (CAMERA_MOVE_FACTOR * Global::dt));

        if (keys[SDL_SCANCODE_C] && is_available(SDL_SCANCODE_C)) {
            if (m_scene->depth_map_visualiser_index == POINT)
                m_scene->depth_map_visualiser_index = DIRECTIONAL - 1;
            else
                m_scene->depth_map_visualiser_index++;
        }

        if (Global::key_state[SDL_SCANCODE_Z] && is_available(SDL_SCANCODE_Z))
            m_scene->FBO_visualiser_index = (m_scene->FBO_visualiser_index + 1) % 4;

        if (Global::key_state[SDL_SCANCODE_X] && is_available(SDL_SCANCODE_X))
            m_scene->debug_enabled = !m_scene->debug_enabled;

        if (Global::key_state[SDL_SCANCODE_P] && is_available(SDL_SCANCODE_P))
            paused = !paused;

        if ((SDL_GetModState() & KMOD_SHIFT) != KMOD_NONE && keys[SDL_SCANCODE_PERIOD] &&
            is_available(SDL_SCANCODE_PERIOD))
            playback_speed = playback_speed < 2.0f
                ? playback_speed + 0.10f
                : playback_speed;
        if ((SDL_GetModState() & KMOD_SHIFT) != KMOD_NONE && keys[SDL_SCANCODE_COMMA] &&
            is_available(SDL_SCANCODE_COMMA))
            playback_speed = playback_speed > 0.5f
                ? playback_speed - 0.10f
                : playback_speed;

        if (Global::key_state[SDL_SCANCODE_F] && is_available(SDL_SCANCODE_F))
            m_scene->launch_ball();

        if (Global::key_state[SDL_SCANCODE_LEFT])
            m_scene->m_flipper_left->swing();
        if (Global::key_state[SDL_SCANCODE_RIGHT])
            m_scene->m_flipper_right->swing();
    }
}

bool App::is_available(SDL_Scancode key) {
    try {
        if (m_key_timers.at(key) == 0.0f) {
            m_key_timers.at(key) = 1.0f;
            return true;
        }
    } catch (std::out_of_range& e) {
        m_key_timers.insert({ key, 1.0f });
        return true;
    }

    return false;
}

void App::update() {
    if (!paused)
        m_scene->update();

    for (auto&[key, timer]: m_key_timers) {
        timer -= Global::dt * 5.0f;
        timer = timer < 0.0f ? 0.0f : timer;
    }
}

void App::done() {
    m_scene->done();

    for (auto& m : Model_Library::loaded) {
        m.second->done();
        m.second.reset();
    }

    for (auto& s : Shader_Library::shaders) {
        s.second->done();
        s.second.reset();
    }

    gltTerminate();
    SDL_GL_DeleteContext(m_gl_context);
    SDL_DestroyWindow(m_sdl_window);
    SDL_Quit();

    m_gl_context = nullptr;
    m_sdl_window = nullptr;
}
