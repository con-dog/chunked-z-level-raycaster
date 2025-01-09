#ifndef TEXTURES_SETUP
#define TEXTURES_SETUP

#include <string.h>

#include <cjson/cJSON.h>
#include <SDL3/SDL_render.h>

#include "../../io/read-manifest.h"
#include "./constants.h"
#include "./setup.h"
#include "./types.h"

extern Texture *setup_engine_textures(const char *root_manifest_file);

#endif