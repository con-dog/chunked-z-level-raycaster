#ifndef TEXTURES_TYPES_H
#define TEXTURES_TYPES_H

#include <stdbool.h>
#include <stddef.h>

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>

typedef struct Frame_Src_Container {
  char **data;
  size_t length;
} Frame_Src_Container;

typedef struct Texture_Src_Container {
  SDL_Texture **data;
  size_t        length;
} Texture_Src_Container;

typedef struct World_Object {
  char                 *name;
  char                 *category;
  char                 *src_directory;
  Frame_Src_Container   frame_src_files;
  Texture_Src_Container textures;
  Uint8                 surface_type;
  Uint8                 collision_mode;
  int                   expected_pixel_width;
  int                   expected_pixel_height;
  bool                  use_scale_mode_nearest;
  int                   current_frame_index;
} World_Object;

typedef struct World_Objects_Container {
  World_Object **data;
  size_t         length;
} World_Objects_Container;

#endif