#ifndef TEXTURES_TYPES_H
#define TEXTURES_TYPES_H

#include <stdbool.h>
#include <stddef.h>

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>

typedef struct World_Object
{
  char *name;
  char *src_directory;
  char **frame_src_files;
  char *category;
  Uint8 surface_type;
  Uint8 collision_mode;
  int expected_pixel_width;
  int expected_pixel_height;
  bool use_scale_mode_nearest;
  SDL_Texture **textures;
  int current_frame_index;
} World_Object;

typedef struct World_Objects_Container
{
  World_Object **data;
  size_t length;
} World_Objects_Container;

typedef const struct Pixel_Image_Texture_Asset
{
  unsigned int width;
  unsigned int height;
  unsigned int bytes_per_pixel;
  unsigned char pixel_data[64 * 64 * 4 + 1];
} Pixel_Image_Texture_Asset;

#endif