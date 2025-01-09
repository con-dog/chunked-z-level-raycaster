#include "./setup.h"

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

bool parse_binary_string(const char *str, uint8_t *result)
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

void parse_asset_manifest_json_string(const char *json_string)
{
  const cJSON *texture = NULL;
  const cJSON *texture_data_array = NULL;
  cJSON *root = cJSON_Parse(json_string);
  if (!root)
  {
    const char *error = cJSON_GetErrorPtr();
    if (error)
    {
      fprintf(stderr, "Error parsing JSON: %s\n", error);
    }
  }

  // Process data array
  texture_data_array = cJSON_GetObjectItemCaseSensitive(root, "data");
  int texture_count = cJSON_GetArraySize(texture_data_array);
  printf("Array length is: %d\n", texture_count);

  if (texture_count <= 0)
  {
    // TODO fail.
  }

  // Maybe pass this in as an out_param?
  Texture *textures[texture_count];
  int index = 0;

  cJSON_ArrayForEach(texture, texture_data_array)
  {
    cJSON *name = cJSON_GetObjectItemCaseSensitive(texture, "name");
    cJSON *path = cJSON_GetObjectItemCaseSensitive(texture, "path");
    cJSON *category = cJSON_GetObjectItemCaseSensitive(texture, "category");
    cJSON *surface_type = cJSON_GetObjectItemCaseSensitive(texture, "surface_type");
    cJSON *expected_pixel_width = cJSON_GetObjectItemCaseSensitive(texture, "expected_pixel_width");
    cJSON *expected_pixel_height = cJSON_GetObjectItemCaseSensitive(texture, "expected_pixel_height");
    cJSON *use_scale_mode_nearest = cJSON_GetObjectItemCaseSensitive(texture, "use_scale_mode_nearest");
    cJSON *is_collision_enabled = cJSON_GetObjectItemCaseSensitive(texture, "is_collision_enabled");

    // TODO! validation for all members of object....
    if (!(cJSON_IsString(name) && name->valuestring != NULL))
    {
    }

    // TODO handle textures and texture pointer freeing
    textures[index] = malloc(sizeof(Texture));

    // After validation
    {
      textures[index]->name = strdup(name->valuestring);         // TODO! Handle allocation failures
      textures[index]->path = strdup(path->valuestring);         // TODO! Handle allocation failures
      textures[index]->category = strdup(category->valuestring); // TODO! Handle allocation failures
      // textures[index]->surface_type = strdup(surface_type->valuestring); // TODO! Handle allocation failures
      const char *binary_str = surface_type->valuestring;
      Uint8 result;
      parse_binary_string(binary_str, &result);
      textures[index]->surface_type = result;
      textures[index]->expected_pixel_height = expected_pixel_height->valuedouble;
      textures[index]->expected_pixel_width = expected_pixel_width->valuedouble;
      textures[index]->use_scale_mode_nearest = cJSON_IsTrue(use_scale_mode_nearest);
      textures[index]->is_collision_enabled = cJSON_IsTrue(is_collision_enabled);
    }

    cJSON_Delete(root);
  }

  printf("Texture 0 data %s\n%s\n%s\n%d\n%d\n", textures[0]->name, textures[0]->path, textures[0]->category, textures[0]->surface_type, textures[0]->use_scale_mode_nearest);
}

// void parse_texture_manifest()
// {
//   char *manifest_version = NULL;

//   cJSON *metadata = cJSON_CreateObject();
//   if (metadata == NULL)
//   {
//     // TODO update
//     printf("Couldn't create metadata");
//   }
// }

// Get passed root manifest
// Parse manifest
// Parse pngs
// Create data structures

// Pixel_Image_Texture_Asset brick_a, brick_b, brick_c, brick_d;
// Pixel_Image_Texture_Asset lava_a, lava_b, lava_c;
// Pixel_Image_Texture_Asset mud_brick_a, mud_brick_b, mud_brick_c;
// Pixel_Image_Texture_Asset overgrown_a, overgrown_b;
// Pixel_Image_Texture_Asset water_b, water_c;
// Pixel_Image_Texture_Asset wood_vertical;

// SDL_Texture *brick_a_texture, *brick_b_texture, *brick_c_texture, *brick_d_texture;
// SDL_Texture *lava_a_texture, *lava_b_texture, *lava_c_texture;
// SDL_Texture *mud_brick_a_texture, *mud_brick_b_texture, *mud_brick_c_texture;
// SDL_Texture *overgrown_a_texture, *overgrown_b_texture;
// SDL_Texture *water_b_texture, *water_c_texture;
// SDL_Texture *wood_vertical_texture;

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