#include "./setup.h"

extern World_Objects_Container *setup_engine_textures(SDL_Renderer *renderer, char *root_manifest_file)
{
  const char *manifest_json_string = read_asset_manifest_file(root_manifest_file);
  if (!manifest_json_string)
  {
    return NULL;
  }

  World_Objects_Container *world_objects_container = malloc(sizeof(World_Objects_Container));
  if (!world_objects_container)
  {
    free((void *)manifest_json_string);
    return NULL;
  }

  if (!parse_asset_manifest_json_string(world_objects_container, manifest_json_string))
  {
    free((void *)manifest_json_string);
    free(world_objects_container);
    return NULL;
  }

  free((void *)manifest_json_string);

  if (!process_textures(renderer, world_objects_container))
  {
    cleanup_world_objects(world_objects_container);
    return NULL;
  }

  return world_objects_container;
}

bool parse_asset_manifest_json_string(World_Objects_Container *out_world_objects_container, const char *json_string)
{
  cJSON *root = cJSON_Parse(json_string);
  if (!root) // TODO ! Move to validation func
  {
    const char *error = cJSON_GetErrorPtr();
    if (error)
    {
      fprintf(stderr, "Error parsing JSON: %s\n", error);
    }
    return false;
  }

  cJSON *world_object_data = cJSON_GetObjectItemCaseSensitive(root, "data");
  if (!world_object_data || !cJSON_IsArray(world_object_data)) // TODO ! Move to validation func
  {
    cJSON_Delete(root);
    return false;
  }

  int world_object_count = cJSON_GetArraySize(world_object_data);
  if (world_object_count <= 0) // TODO ! Move to validation func
  {
    cJSON_Delete(root);
    return false;
  }

  out_world_objects_container->data = malloc(world_object_count * sizeof(World_Object *));
  if (!out_world_objects_container->data) // TODO ! Move to validation func
  {
    cJSON_Delete(root);
    return false;
  }
  out_world_objects_container->length = world_object_count;

  // Initialize all pointers to NULL for safe cleanup on failure
  for (int i = 0; i < world_object_count; i++)
  {
    out_world_objects_container->data[i] = NULL;
  }

  cJSON *world_object = NULL;
  int world_object_index = 0;
  cJSON_ArrayForEach(world_object, world_object_data)
  {
    World_Object *current_world_object = malloc(sizeof(World_Object));
    if (!current_world_object) // TODO ! Move to validation func
    {
      cJSON_Delete(root);
      cleanup_world_objects(out_world_objects_container);
      return false;
    }
    out_world_objects_container->data[world_object_index] = current_world_object;

    if (!parse_texture_fields(current_world_object, world_object))
    {
      cJSON_Delete(root);
      cleanup_world_objects(out_world_objects_container);
      return false;
    }

    world_object_index++;
  }

  cJSON_Delete(root);
  return true;
}

bool parse_frame_src_files(World_Object *world_object, cJSON *frame_src_files_array)
{
  if (!frame_src_files_array || !cJSON_IsArray(frame_src_files_array))
  {
    return false;
  }

  int frame_count = cJSON_GetArraySize(frame_src_files_array);
  if (frame_count <= 0)
  {
    return false;
  }

  // Allocate array of char pointers
  world_object->frame_src_files.data = malloc(frame_count * sizeof(char *));
  if (!world_object->frame_src_files.data)
  {
    return false;
  }
  world_object->frame_src_files.length = frame_count;

  // Allocate array of SDL_Texture pointers
  world_object->textures.data = malloc(frame_count * sizeof(SDL_Texture));
  if (!world_object->textures.data)
  {
    return false;
  }
  world_object->textures.length = frame_count;

  // Initialize all pointers to NULL for safe cleanup
  for (int i = 0; i <= frame_count; i++)
  {
    world_object->frame_src_files.data[i] = NULL;
    world_object->textures.data[i] = NULL;
  }

  // Parse each frame source file
  for (int i = 0; i < frame_count; i++)
  {
    cJSON *frame_src = cJSON_GetArrayItem(frame_src_files_array, i);
    if (!frame_src || !cJSON_IsString(frame_src))
    {
      return false;
    }

    const char *frame_src_str = cJSON_GetStringValue(frame_src);
    if (!frame_src_str)
    {
      return false;
    }

    world_object->frame_src_files.data[i] = strdup(frame_src_str);
    if (!world_object->frame_src_files.data[i])
    {
      return false;
    }
  }

  return true;
}

bool parse_texture_fields(World_Object *world_object, const cJSON *json_object)
{
  // Parse name
  cJSON *name = cJSON_GetObjectItemCaseSensitive(json_object, "name");
  if (!name || !cJSON_IsString(name))
  {
    return false;
  }
  world_object->name = strdup(cJSON_GetStringValue(name));
  if (!world_object->name)
  {
    return false;
  }

  // Parse src_directory
  cJSON *src_directory = cJSON_GetObjectItemCaseSensitive(json_object, "src_directory");
  if (!src_directory || !cJSON_IsString(src_directory))
  {
    return false;
  }
  world_object->src_directory = strdup(cJSON_GetStringValue(src_directory));
  if (!world_object->src_directory)
  {
    return false;
  }

  // Parse frame_src_files array
  cJSON *frame_src_files = cJSON_GetObjectItemCaseSensitive(json_object, "frame_src_files");
  if (!parse_frame_src_files(world_object, frame_src_files))
  {
    return false;
  }

  // Parse category
  cJSON *category = cJSON_GetObjectItemCaseSensitive(json_object, "category");
  if (!category || !cJSON_IsString(category))
  {
    return false;
  }
  world_object->category = strdup(cJSON_GetStringValue(category));
  if (!world_object->category)
  {
    return false;
  }

  // Parse surface_type and collision_mode
  cJSON *surface_type = cJSON_GetObjectItemCaseSensitive(json_object, "surface_type");
  cJSON *collision_mode = cJSON_GetObjectItemCaseSensitive(json_object, "collision_mode");
  if (!surface_type || !collision_mode)
  {
    return false;
  }
  // Convert string binary representation to integer
  const char *surface_type_str = cJSON_GetStringValue(surface_type);
  const char *collision_mode_str = cJSON_GetStringValue(collision_mode);
  if (!surface_type_str || !collision_mode_str)
  {
    return false;
  }
  // Skip "0b" prefix and convert to integer
  world_object->surface_type = (Uint8)strtol(surface_type_str + 2, NULL, 2);
  world_object->collision_mode = (Uint8)strtol(collision_mode_str + 2, NULL, 2);

  // Parse dimensions
  cJSON *width = cJSON_GetObjectItemCaseSensitive(json_object, "expected_pixel_width");
  cJSON *height = cJSON_GetObjectItemCaseSensitive(json_object, "expected_pixel_height");
  if (!width || !height || !cJSON_IsNumber(width) || !cJSON_IsNumber(height))
  {
    return false;
  }
  world_object->expected_pixel_width = width->valueint;
  world_object->expected_pixel_height = height->valueint;

  // Parse scale mode
  cJSON *scale_mode = cJSON_GetObjectItemCaseSensitive(json_object, "use_scale_mode_nearest");
  if (!scale_mode || !cJSON_IsBool(scale_mode))
  {
    return false;
  }
  world_object->use_scale_mode_nearest = cJSON_IsTrue(scale_mode);

  // Initialize other fields
  world_object->current_frame_index = 0;

  return true;
}

bool process_textures(SDL_Renderer *renderer, World_Objects_Container *out_world_objects_container)
{
  if (!renderer || !out_world_objects_container)
  {
    return false;
  }

  for (size_t i = 0; i < out_world_objects_container->length; i++)
  {
    for (size_t j = 0; j < out_world_objects_container->data[i]->frame_src_files.length; j++)
    {
      char temp_path[MAX_PATH_LENGTH];
      int chars_written = snprintf(temp_path, MAX_PATH_LENGTH, "%s/%s",
                                   out_world_objects_container->data[i]->src_directory,
                                   out_world_objects_container->data[i]->frame_src_files.data[j]);

      if (chars_written >= MAX_PATH_LENGTH)
      {
        printf("Error: Truncation in path occured");
      }

      SDL_Surface *temp_surface = IMG_Load(temp_path);

      // SDL_Surface *temp_surface = IMG_Load(out_world_objects_container->data[i]->path);

      if (!temp_surface)
      {
        fprintf(stderr, "Failed to load image %s\n",
                temp_path);
        return false;
      }

      SDL_Texture *temp_texture = SDL_CreateTextureFromSurface(renderer, temp_surface);
      SDL_DestroySurface(temp_surface);

      if (!temp_texture)
      {
        fprintf(stderr, "Failed to create texture from %s: %s\n",
                temp_path, SDL_GetError());
        return false;
      }

      SDL_ScaleMode scale_mode = out_world_objects_container->data[i]->use_scale_mode_nearest
                                     ? SDL_SCALEMODE_NEAREST
                                     : SDL_SCALEMODE_LINEAR;

      if (!SDL_SetTextureScaleMode(temp_texture, scale_mode))
      {
        fprintf(stderr, "Failed to set texture scale mode: %s\n", SDL_GetError());
        SDL_DestroyTexture(temp_texture);
        return false;
      }

      out_world_objects_container->data[i]->textures.data[j] = temp_texture;
    }
  }

  return true;
}

void cleanup_world_objects(World_Objects_Container *container)
{
  if (!container || !container->data)
  {
    return;
  }

  for (size_t i = 0; i < container->length; i++)
  {
    cleanup_world_object(container->data[i]);
  }

  free(container->data);
  container->data = NULL;
  container->length = 0;
}

void cleanup_world_object(World_Object *world_object)
{
  if (!world_object)
  {
    return;
  }

  free(world_object->name);
  free(world_object->src_directory);
  free(world_object->category);

  // Clean up frame source files using the container cleanup
  cleanup_frame_src_container(&world_object->frame_src_files);
  cleanup_textures(&world_object->textures);

  free(world_object);
}

void cleanup_frame_src_container(Frame_Src_Container *container)
{
  if (!container || !container->data)
  {
    return;
  }

  for (size_t i = 0; i < container->length; i++)
  {
    free(container->data[i]);
  }
  free(container->data);
  container->data = NULL;
  container->length = 0;
}

void cleanup_textures(Texture_Src_Container *container)
{
  if (!container || !container->data)
  {
    return;
  }

  for (size_t i = 0; i < container->length; i++)
  {
    SDL_DestroyTexture(container->data[i]);
  }

  free(container->data);
  container->data = NULL;
  container->length = 0;
}
