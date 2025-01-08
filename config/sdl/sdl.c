#include "./sdl.h"

static int initialize_sdl_video(void);
static int initialize_window_and_renderer(const char *title, int width, int height, SDL_WindowFlags window_flags, SDL_Window **window, SDL_Renderer **renderer);

extern int setup_sdl(const char *title, int width, int height, SDL_WindowFlags window_flags, SDL_Window **out_window, SDL_Renderer **out_renderer)
{
  initialize_sdl_video();
  initialize_window_and_renderer(title, width, height, window_flags, out_window, out_renderer);
  return 0;
}

static int initialize_sdl_video()
{
  if (!SDL_Init(SDL_INIT_VIDEO))
  {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL_INIT_VIDEO: %s", SDL_GetError());
    return 3;
  }
  return 0;
}

static int initialize_window_and_renderer(const char *title, int width, int height, SDL_WindowFlags window_flags, SDL_Window **window, SDL_Renderer **renderer)
{
  if (!SDL_CreateWindowAndRenderer(title, width, height, window_flags, window, renderer))
  {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window and renderer: %s", SDL_GetError());
    return 3;
  }

  return 0;
}
