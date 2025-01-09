#ifndef TEXTURES_TYPES_H
#define TEXTURES_TYPES_H

#include <stdbool.h>

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>

typedef struct Texture
{
  char *name;
  char *path;
  char *category;
  Uint8 surface_type;
  int expected_pixel_width;
  int expected_pixel_height;
  bool use_scale_mode_nearest;
  bool is_collision_enabled;
  SDL_Texture *texture;
} Texture;

typedef const struct Pixel_Image_Texture_Asset
{
  unsigned int width;
  unsigned int height;
  unsigned int bytes_per_pixel;
  unsigned char pixel_data[64 * 64 * 4 + 1];
} Pixel_Image_Texture_Asset;

#endif