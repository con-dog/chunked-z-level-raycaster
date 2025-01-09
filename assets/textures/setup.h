#ifndef TEXTURES_SETUP
#define TEXTURES_SETUP

#include <cjson/cJSON.h>
#include <SDL3/SDL_render.h>

#include "../../io/read-manifest.h"
#include "./constants.h"
#include "./setup.h"
#include "./types.h"

extern void parse_asset_manifest_json_string(const char *json_string);

#endif