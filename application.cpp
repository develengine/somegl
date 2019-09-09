#include "application.hpp"

#include <glad/glad.h>

#include <iostream>

namespace Application {

SDL_Window *window = nullptr;
SDL_GLContext context;
bool running = true;

std::function<void(SDL_Event&, bool)> keyCallback = nullptr;
std::function<void(SDL_Event&)> mouseMotionCallback = nullptr;

void setKeyCallback(std::function<void(SDL_Event&, bool)> f) {
    keyCallback = f;
}

void setMouseMotionCallback(std::function<void(SDL_Event&)> f) {
    mouseMotionCallback = f;
}

SDL_Event event;
void pollEvents() {
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                running = false;
                break;
            case SDL_WINDOWEVENT:
                switch (event.window.event) {
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        glViewport(0, 0, event.window.data1, event.window.data2);
                        break;
                }
                break;
            case SDL_KEYDOWN:
                if (keyCallback != nullptr) {
                    keyCallback(event, true);
                }
                break;
            case SDL_KEYUP:
                if (keyCallback != nullptr) {
                    keyCallback(event, false);
                }
                break;
            case SDL_MOUSEMOTION:
                if (mouseMotionCallback != nullptr) {
                    mouseMotionCallback(event);
                }
                break;
        }
    }
}

void init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Failed to initialize SDL2\n";
        exit(-1);
    }
    SDL_GL_LoadLibrary(NULL);

    window = SDL_CreateWindow(
        "bones.",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1080, 720,
        SDL_WINDOW_OPENGL
    );

    SDL_SetWindowResizable(window, SDL_TRUE);

    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    context = SDL_GL_CreateContext(window);
    if (context == NULL) {
        std::cerr << "Failed to initialize context\n";
        exit(-1);
    }

    if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
        std::cerr << "Failed to load GLAD\n";
        exit(-1);
    }

    SDL_GL_SetSwapInterval(1);
    SDL_SetRelativeMouseMode(SDL_TRUE);
}

void close() {
    SDL_DestroyWindow(window);
    SDL_Quit();
}

}
