#ifndef CONFIG_SDL_H
#define CONFIG_SDL_H

#include <SDL3/SDL_render.h>
#include <SDL3/SDL.h>

extern int setup_sdl(const char *title, int width, int height, SDL_WindowFlags window_flags, SDL_Window **out_window, SDL_Renderer **out_renderer);

#endif