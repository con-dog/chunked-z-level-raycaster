#include "./setup.h"

typedef struct Texture
{
  char *name;
  char *path;
  char *category;
  Uint8 surface_type;
  int expected_pixel_width;
  int expected_pixel_height;
  bool scale_mode;
  bool is_collision_enabled;
  SDL_Texture *texture;
} Texture;

void parse_asset_manifest_json_string(const char *json_string)
{
  const cJSON *texture = NULL;
  const cJSON *textures = NULL;
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
  textures = cJSON_GetObjectItemCaseSensitive(root, "data");
  int texture_count = cJSON_GetArraySize(textures);
  printf("Array length is: %d\n", texture_count);

  cJSON_ArrayForEach(texture, textures)
  {
    cJSON *name = cJSON_GetObjectItemCaseSensitive(texture, "name");
    cJSON *path = cJSON_GetObjectItemCaseSensitive(texture, "path");
    cJSON *category = cJSON_GetObjectItemCaseSensitive(texture, "category");
    cJSON *surface_type = cJSON_GetObjectItemCaseSensitive(texture, "surface_type");
    cJSON *expected_pixel_width = cJSON_GetObjectItemCaseSensitive(texture, "expected_pixel_width");
    cJSON *expected_pixel_height = cJSON_GetObjectItemCaseSensitive(texture, "expected_pixel_height");
    cJSON *scale_mode = cJSON_GetObjectItemCaseSensitive(texture, "scale_mode");
    cJSON *is_collision_enabled = cJSON_GetObjectItemCaseSensitive(texture, "is_collision_enabled");

    // validation
    if (cJSON_IsString(name) && name->valuestring != NULL)
    {
    }
  }

  cJSON_Delete(root);
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