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

bool parse_asset_manifest_json_string(World_Objects_Container *out_world_objects_container, const char *json_string);
bool parse_texture_fields(World_Object *world_object, const cJSON *json_object);
bool parse_frame_src_files(World_Object *world_object, cJSON *frame_src_files_array);
bool process_world_objects(SDL_Renderer *renderer, World_Objects_Container *out_world_objects_container);
void cleanup_world_objects(World_Objects_Container *container);
void cleanup_world_object(World_Object *world_object);
void cleanup_frame_src_container(Frame_Src_Container *container);
void cleanup_textures(Texture_Src_Container *container);

#endif