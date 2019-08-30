#pragma once

#include <SDL2/SDL.h>

namespace Application {

extern SDL_Window *window;
extern SDL_GLContext context;
extern bool running;

void init();
void pollEvents();
void close();

}
