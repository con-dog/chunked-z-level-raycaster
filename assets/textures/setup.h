#ifndef TEXTURES_SETUP
#define TEXTURES_SETUP

#include <string.h>

#include <cjson/cJSON.h>
#include <SDL3/SDL_render.h>
#include <SDL3_image/SDL_image.h>

#include "./constants.h"
#include "./setup.h"
#include "./types.h"
#include "../../io/read-manifest.h"

extern World_Objects_Container *setup_engine_textures(SDL_Renderer *renderer, char *root_manifest_file);
extern void cleanup_textures(World_Objects_Container *textures);

#endif