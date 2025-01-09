#include "./setup.h"

static bool parse_asset_manifest_json_string(World_Objects_Container *out_array_list, const char *json_string);
static bool parse_texture_fields(World_Object *texture, const cJSON *json);
static void cleanup_texture(World_Object *texture);
static bool validate_texture_fields(const cJSON *name, const cJSON *path,
                                    const cJSON *category, const cJSON *surface_type,
                                    const cJSON *width, const cJSON *height);
static bool parse_binary_string(const char *str, uint8_t *result);

static bool process_textures(SDL_Renderer *renderer, World_Objects_Container *out_array_list);

extern World_Objects_Container *setup_engine_textures(SDL_Renderer *renderer, char *root_manifest_file)
{
  const char *manifest_json_string = read_asset_manifest_file(root_manifest_file);
  if (!manifest_json_string)
  {
    return NULL;
  }

  World_Objects_Container *textures_list = malloc(sizeof(World_Objects_Container));
  if (!textures_list)
  {
    free((void *)manifest_json_string);
    return NULL;
  }

  if (!parse_asset_manifest_json_string(textures_list, manifest_json_string))
  {
    free((void *)manifest_json_string);
    free(textures_list);
    return NULL;
  }

  free((void *)manifest_json_string);

  if (!process_textures(renderer, textures_list))
  {
    cleanup_textures(textures_list);
    return NULL;
  }

  return textures_list;
}

extern void cleanup_textures(World_Objects_Container *textures)
{
  if (!textures)
    return;
  if (textures->data)
  {
    for (size_t i = 0; i < textures->length; i++)
    {
      if (textures->data[i])
      {
        cleanup_texture(textures->data[i]);
        free(textures->data[i]);
      }
    }
    free(textures->data);
  }
  free(textures);
}

static bool process_textures(SDL_Renderer *renderer, World_Objects_Container *out_array_list)
{
  if (!renderer || !out_array_list)
    return false;

  for (int i = 0; i < out_array_list->length; i++)
  {
    SDL_Surface *temp_surface = IMG_Load(out_array_list->data[i]->path);
    if (!temp_surface)
    {
      fprintf(stderr, "Failed to load image %s\n",
              out_array_list->data[i]->path);
      return false;
    }

    SDL_Texture *temp_texture = SDL_CreateTextureFromSurface(renderer, temp_surface);
    SDL_DestroySurface(temp_surface);

    if (!temp_texture)
    {
      fprintf(stderr, "Failed to create texture from %s: %s\n",
              out_array_list->data[i]->path, SDL_GetError());
      return false;
    }

    SDL_ScaleMode scale_mode = out_array_list->data[i]->use_scale_mode_nearest
                                   ? SDL_SCALEMODE_NEAREST
                                   : SDL_SCALEMODE_LINEAR;

    if (!SDL_SetTextureScaleMode(temp_texture, scale_mode))
    {
      fprintf(stderr, "Failed to set texture scale mode: %s\n", SDL_GetError());
      SDL_DestroyTexture(temp_texture);
      return false;
    }

    out_array_list->data[i]->texture = temp_texture;
  }

  return true;
}

static bool parse_asset_manifest_json_string(World_Objects_Container *out_array_list, const char *json_string)
{
  cJSON *root = cJSON_Parse(json_string);
  if (!root)
  {
    const char *error = cJSON_GetErrorPtr();
    if (error)
    {
      fprintf(stderr, "Error parsing JSON: %s\n", error);
    }
    return false;
  }

  cJSON *texture_data_array = cJSON_GetObjectItemCaseSensitive(root, "data");
  if (!texture_data_array || !cJSON_IsArray(texture_data_array))
  {
    cJSON_Delete(root);
    return false;
  }

  int texture_count = cJSON_GetArraySize(texture_data_array);
  if (texture_count <= 0)
  {
    cJSON_Delete(root);
    return false;
  }

  out_array_list->data = malloc(texture_count * sizeof(World_Object *));
  if (!out_array_list->data)
  {
    cJSON_Delete(root);
    return false;
  }
  out_array_list->length = texture_count;

  // Initialize all pointers to NULL for safe cleanup on failure
  for (int i = 0; i < texture_count; i++)
  {
    out_array_list->data[i] = NULL;
  }

  cJSON *texture = NULL;
  int index = 0;
  cJSON_ArrayForEach(texture, texture_data_array)
  {
    World_Object *current_texture = malloc(sizeof(World_Object));
    if (!current_texture)
    {
      cJSON_Delete(root);
      cleanup_textures(out_array_list);
      return false;
    }
    out_array_list->data[index] = current_texture;

    // Initialize texture fields to NULL/0
    current_texture->name = NULL;
    current_texture->path = NULL;
    current_texture->category = NULL;
    current_texture->texture = NULL;

    if (!parse_texture_fields(current_texture, texture))
    {
      cJSON_Delete(root);
      cleanup_textures(out_array_list);
      return false;
    }
    index++;
  }

  cJSON_Delete(root);
  return true;
}

static bool parse_texture_fields(World_Object *texture, const cJSON *json)
{
  cJSON *name = cJSON_GetObjectItemCaseSensitive(json, "name");
  cJSON *path = cJSON_GetObjectItemCaseSensitive(json, "path");
  cJSON *category = cJSON_GetObjectItemCaseSensitive(json, "category");
  cJSON *surface_type = cJSON_GetObjectItemCaseSensitive(json, "surface_type");
  cJSON *expected_pixel_width = cJSON_GetObjectItemCaseSensitive(json, "expected_pixel_width");
  cJSON *expected_pixel_height = cJSON_GetObjectItemCaseSensitive(json, "expected_pixel_height");
  cJSON *use_scale_mode_nearest = cJSON_GetObjectItemCaseSensitive(json, "use_scale_mode_nearest");
  cJSON *is_collision_enabled = cJSON_GetObjectItemCaseSensitive(json, "is_collision_enabled");

  if (!validate_texture_fields(name, path, category, surface_type,
                               expected_pixel_width, expected_pixel_height))
  {
    return false;
  }

  texture->name = strdup(name->valuestring);
  texture->path = strdup(path->valuestring);
  texture->category = strdup(category->valuestring);

  if (!texture->name || !texture->path || !texture->category)
  {
    cleanup_texture(texture);
    return false;
  }

  Uint8 result;
  if (!parse_binary_string(surface_type->valuestring, &result))
  {
    cleanup_texture(texture);
    return false;
  }

  texture->surface_type = result;
  texture->expected_pixel_height = expected_pixel_height->valuedouble;
  texture->expected_pixel_width = expected_pixel_width->valuedouble;
  texture->use_scale_mode_nearest = cJSON_IsTrue(use_scale_mode_nearest);
  texture->is_collision_enabled = cJSON_IsTrue(is_collision_enabled);

  return true;
}

static void cleanup_texture(World_Object *texture)
{
  if (!texture)
    return;
  free(texture->name);
  free(texture->path);
  free(texture->category);
  if (texture->texture)
  {
    SDL_DestroyTexture(texture->texture);
  }
}

static bool validate_texture_fields(const cJSON *name, const cJSON *path,
                                    const cJSON *category, const cJSON *surface_type,
                                    const cJSON *width, const cJSON *height)
{
  return (cJSON_IsString(name) && name->valuestring &&
          cJSON_IsString(path) && path->valuestring &&
          cJSON_IsString(category) && category->valuestring &&
          cJSON_IsString(surface_type) && surface_type->valuestring &&
          cJSON_IsNumber(width) && cJSON_IsNumber(height));
}

static bool parse_binary_string(const char *str, uint8_t *result)
{
  uint8_t value = 0;
  size_t bits = 0;

  if (str[0] != '0' || str[1] != 'b')
  {
    return false;
  }

  for (const char *p = str + 2; *p; p++)
  {
    if (*p != '0' && *p != '1')
    {
      return false;
    }
    if (bits >= 8)
    {
      return false; // Overflow
    }
    value = (value << 1) | (uint8_t)(*p - '0');
    bits++;
  }

  *result = value;
  return true;
}

// SDL_Texture *shotgun_1_texture;

// static int brick_texture_init(void)
// {

//   brick_a_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, TEXTURE_PIXEL_W, TEXTURE_PIXEL_H);
//   if (brick_a_texture == NULL)
//   {
//     SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Texture could not initialize! SDL_Texture Error: %s\n", SDL_GetError());
//     return 3;
//   }
//   brick_b_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, TEXTURE_PIXEL_W, TEXTURE_PIXEL_H);
//   if (brick_b_texture == NULL)
//   {
//     SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Texture could not initialize! SDL_Texture Error: %s\n", SDL_GetError());
//     return 3;
//   }
//   brick_c_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, TEXTURE_PIXEL_W, TEXTURE_PIXEL_H);
//   if (brick_c_texture == NULL)
//   {
//     SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Texture could not initialize! SDL_Texture Error: %s\n", SDL_GetError());
//     return 3;
//   }
//   brick_d_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, TEXTURE_PIXEL_W, TEXTURE_PIXEL_H);
//   if (brick_d_texture == NULL)
//   {
//     SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Texture could not initialize! SDL_Texture Error: %s\n", SDL_GetError());
//     return 3;
//   }

//   lava_a_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, TEXTURE_PIXEL_W, TEXTURE_PIXEL_H);
//   lava_b_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, TEXTURE_PIXEL_W, TEXTURE_PIXEL_H);
//   lava_c_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, TEXTURE_PIXEL_W, TEXTURE_PIXEL_H);

//   mud_brick_a_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, TEXTURE_PIXEL_W, TEXTURE_PIXEL_H);
//   mud_brick_b_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, TEXTURE_PIXEL_W, TEXTURE_PIXEL_H);
//   mud_brick_c_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, TEXTURE_PIXEL_W, TEXTURE_PIXEL_H);

//   overgrown_a_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, TEXTURE_PIXEL_W, TEXTURE_PIXEL_H);
//   overgrown_b_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, TEXTURE_PIXEL_W, TEXTURE_PIXEL_H);

//   water_b_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, TEXTURE_PIXEL_W, TEXTURE_PIXEL_H);
//   water_c_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, TEXTURE_PIXEL_W, TEXTURE_PIXEL_H);

//   wood_vertical_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, TEXTURE_PIXEL_W, TEXTURE_PIXEL_H);

//   SDL_SetTextureScaleMode(brick_a_texture, SDL_SCALEMODE_NEAREST);
//   SDL_SetTextureScaleMode(brick_b_texture, SDL_SCALEMODE_NEAREST);
//   SDL_SetTextureScaleMode(brick_c_texture, SDL_SCALEMODE_NEAREST);
//   SDL_SetTextureScaleMode(brick_d_texture, SDL_SCALEMODE_NEAREST);
//   SDL_SetTextureScaleMode(lava_a_texture, SDL_SCALEMODE_NEAREST);
//   SDL_SetTextureScaleMode(lava_b_texture, SDL_SCALEMODE_NEAREST);
//   SDL_SetTextureScaleMode(lava_c_texture, SDL_SCALEMODE_NEAREST);
//   SDL_SetTextureScaleMode(mud_brick_a_texture, SDL_SCALEMODE_NEAREST);
//   SDL_SetTextureScaleMode(mud_brick_b_texture, SDL_SCALEMODE_NEAREST);
//   SDL_SetTextureScaleMode(mud_brick_c_texture, SDL_SCALEMODE_NEAREST);
//   SDL_SetTextureScaleMode(overgrown_a_texture, SDL_SCALEMODE_NEAREST);
//   SDL_SetTextureScaleMode(overgrown_b_texture, SDL_SCALEMODE_NEAREST);
//   SDL_SetTextureScaleMode(water_b_texture, SDL_SCALEMODE_NEAREST);
//   SDL_SetTextureScaleMode(water_c_texture, SDL_SCALEMODE_NEAREST);
//   SDL_SetTextureScaleMode(wood_vertical_texture, SDL_SCALEMODE_NEAREST);

//   SDL_UpdateTexture(brick_a_texture, NULL, brick_a.pixel_data, TEXTURE_PIXEL_W * 4);
//   SDL_UpdateTexture(brick_b_texture, NULL, brick_b.pixel_data, TEXTURE_PIXEL_W * 4);
//   SDL_UpdateTexture(brick_c_texture, NULL, brick_c.pixel_data, TEXTURE_PIXEL_W * 4);
//   SDL_UpdateTexture(brick_d_texture, NULL, brick_d.pixel_data, TEXTURE_PIXEL_W * 4);
//   SDL_UpdateTexture(lava_a_texture, NULL, lava_a.pixel_data, TEXTURE_PIXEL_W * 4);
//   SDL_UpdateTexture(lava_b_texture, NULL, lava_b.pixel_data, TEXTURE_PIXEL_W * 4);
//   SDL_UpdateTexture(lava_c_texture, NULL, lava_c.pixel_data, TEXTURE_PIXEL_W * 4);
//   SDL_UpdateTexture(mud_brick_a_texture, NULL, mud_brick_a.pixel_data, TEXTURE_PIXEL_W * 4);
//   SDL_UpdateTexture(mud_brick_b_texture, NULL, mud_brick_b.pixel_data, TEXTURE_PIXEL_W * 4);
//   SDL_UpdateTexture(mud_brick_c_texture, NULL, mud_brick_c.pixel_data, TEXTURE_PIXEL_W * 4);
//   SDL_UpdateTexture(overgrown_a_texture, NULL, overgrown_a.pixel_data, TEXTURE_PIXEL_W * 4);
//   SDL_UpdateTexture(overgrown_b_texture, NULL, overgrown_b.pixel_data, TEXTURE_PIXEL_W * 4);
//   SDL_UpdateTexture(water_b_texture, NULL, water_b.pixel_data, TEXTURE_PIXEL_W * 4);
//   SDL_UpdateTexture(water_c_texture, NULL, water_c.pixel_data, TEXTURE_PIXEL_W * 4);
//   SDL_UpdateTexture(wood_vertical_texture, NULL, wood_vertical.pixel_data, TEXTURE_PIXEL_W * 4);

//   SDL_SetTextureScaleMode(shotgun_1_texture, SDL_SCALEMODE_NEAREST);
//   SDL_Surface *shotgun_1_surface = IMG_Load("./assets/sprites/shotgun/shotgun-1/shotgun-1.png");
//   shotgun_1_texture = SDL_CreateTextureFromSurface(renderer, shotgun_1_surface);
//   SDL_DestroySurface(shotgun_1_surface);

//   shotgun_1.texture = shotgun_1_texture.;
//   shotgun_1.height = 304;
//   shotgun_1.width = 444;

//   return 0;
// }