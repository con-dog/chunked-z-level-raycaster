#include "main.h"

SDL_Window *win;
SDL_Renderer *renderer;

Pixel_Image_Asset brick_a, brick_b, brick_c, brick_d;
Pixel_Image_Asset lava_a, lava_b, lava_c;
Pixel_Image_Asset mud_brick_a, mud_brick_b, mud_brick_c;
Pixel_Image_Asset overgrown_a, overgrown_b;
Pixel_Image_Asset water_b, water_c;
Pixel_Image_Asset wood_vertical;

SDL_Texture *brick_a_texture, *brick_b_texture, *brick_c_texture, *brick_d_texture;
SDL_Texture *lava_a_texture, *lava_b_texture, *lava_c_texture;
SDL_Texture *mud_brick_a_texture, *mud_brick_b_texture, *mud_brick_c_texture;
SDL_Texture *overgrown_a_texture, *overgrown_b_texture;
SDL_Texture *water_b_texture, *water_c_texture;
SDL_Texture *wood_vertical_texture;

Jagged_Grid *floor_grid;
Jagged_Grid *wall_grid;

Player player;

const bool *keyboard_state;

static int sdl_init()
{
  if (!SDL_Init(SDL_INIT_VIDEO))
  {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
    return 3;
  }

  if (!TTF_Init())
  {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_ttf could not initialize! TTF_Error: %s\n", SDL_GetError());
    return 3;
  }

  if (!SDL_CreateWindowAndRenderer("2.5D Raycaster", WINDOW_W, WINDOW_H, SDL_WINDOW_RESIZABLE, &win, &renderer))
  {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window and renderer: %s", SDL_GetError());
    return 3;
  }

  return 0;
}

// static int font_init(void)
// {
//   font = TTF_OpenFont("./fonts/PressStart2P-Regular.ttf", FONT_SMALL);
//   if (!font)
//   {
//     SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load font! TTF_Error: %s\n", SDL_GetError());
//     return 3;
//   }
//   return 0;
// }

static int brick_texture_init(void)
{

  brick_a_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, TEXTURE_PIXEL_W, TEXTURE_PIXEL_H);
  if (brick_a_texture == NULL)
  {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Texture could not initialize! SDL_Texture Error: %s\n", SDL_GetError());
    return 3;
  }
  brick_b_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, TEXTURE_PIXEL_W, TEXTURE_PIXEL_H);
  if (brick_b_texture == NULL)
  {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Texture could not initialize! SDL_Texture Error: %s\n", SDL_GetError());
    return 3;
  }
  brick_c_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, TEXTURE_PIXEL_W, TEXTURE_PIXEL_H);
  if (brick_c_texture == NULL)
  {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Texture could not initialize! SDL_Texture Error: %s\n", SDL_GetError());
    return 3;
  }
  brick_d_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, TEXTURE_PIXEL_W, TEXTURE_PIXEL_H);
  if (brick_d_texture == NULL)
  {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Texture could not initialize! SDL_Texture Error: %s\n", SDL_GetError());
    return 3;
  }

  lava_a_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, TEXTURE_PIXEL_W, TEXTURE_PIXEL_H);
  lava_b_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, TEXTURE_PIXEL_W, TEXTURE_PIXEL_H);
  lava_c_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, TEXTURE_PIXEL_W, TEXTURE_PIXEL_H);

  mud_brick_a_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, TEXTURE_PIXEL_W, TEXTURE_PIXEL_H);
  mud_brick_b_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, TEXTURE_PIXEL_W, TEXTURE_PIXEL_H);
  mud_brick_c_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, TEXTURE_PIXEL_W, TEXTURE_PIXEL_H);

  overgrown_a_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, TEXTURE_PIXEL_W, TEXTURE_PIXEL_H);
  overgrown_b_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, TEXTURE_PIXEL_W, TEXTURE_PIXEL_H);

  water_b_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, TEXTURE_PIXEL_W, TEXTURE_PIXEL_H);
  water_c_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, TEXTURE_PIXEL_W, TEXTURE_PIXEL_H);

  wood_vertical_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, TEXTURE_PIXEL_W, TEXTURE_PIXEL_H);

  SDL_SetTextureScaleMode(brick_a_texture, SDL_SCALEMODE_NEAREST);
  SDL_SetTextureScaleMode(brick_b_texture, SDL_SCALEMODE_NEAREST);
  SDL_SetTextureScaleMode(brick_c_texture, SDL_SCALEMODE_NEAREST);
  SDL_SetTextureScaleMode(brick_d_texture, SDL_SCALEMODE_NEAREST);
  SDL_SetTextureScaleMode(lava_a_texture, SDL_SCALEMODE_NEAREST);
  SDL_SetTextureScaleMode(lava_b_texture, SDL_SCALEMODE_NEAREST);
  SDL_SetTextureScaleMode(lava_c_texture, SDL_SCALEMODE_NEAREST);
  SDL_SetTextureScaleMode(mud_brick_a_texture, SDL_SCALEMODE_NEAREST);
  SDL_SetTextureScaleMode(mud_brick_b_texture, SDL_SCALEMODE_NEAREST);
  SDL_SetTextureScaleMode(mud_brick_c_texture, SDL_SCALEMODE_NEAREST);
  SDL_SetTextureScaleMode(overgrown_a_texture, SDL_SCALEMODE_NEAREST);
  SDL_SetTextureScaleMode(overgrown_b_texture, SDL_SCALEMODE_NEAREST);
  SDL_SetTextureScaleMode(water_b_texture, SDL_SCALEMODE_NEAREST);
  SDL_SetTextureScaleMode(water_c_texture, SDL_SCALEMODE_NEAREST);
  SDL_SetTextureScaleMode(wood_vertical_texture, SDL_SCALEMODE_NEAREST);

  SDL_UpdateTexture(brick_a_texture, NULL, brick_a.pixel_data, TEXTURE_PIXEL_W * 4);
  SDL_UpdateTexture(brick_b_texture, NULL, brick_b.pixel_data, TEXTURE_PIXEL_W * 4);
  SDL_UpdateTexture(brick_c_texture, NULL, brick_c.pixel_data, TEXTURE_PIXEL_W * 4);
  SDL_UpdateTexture(brick_d_texture, NULL, brick_d.pixel_data, TEXTURE_PIXEL_W * 4);
  SDL_UpdateTexture(lava_a_texture, NULL, lava_a.pixel_data, TEXTURE_PIXEL_W * 4);
  SDL_UpdateTexture(lava_b_texture, NULL, lava_b.pixel_data, TEXTURE_PIXEL_W * 4);
  SDL_UpdateTexture(lava_c_texture, NULL, lava_c.pixel_data, TEXTURE_PIXEL_W * 4);
  SDL_UpdateTexture(mud_brick_a_texture, NULL, mud_brick_a.pixel_data, TEXTURE_PIXEL_W * 4);
  SDL_UpdateTexture(mud_brick_b_texture, NULL, mud_brick_b.pixel_data, TEXTURE_PIXEL_W * 4);
  SDL_UpdateTexture(mud_brick_c_texture, NULL, mud_brick_c.pixel_data, TEXTURE_PIXEL_W * 4);
  SDL_UpdateTexture(overgrown_a_texture, NULL, overgrown_a.pixel_data, TEXTURE_PIXEL_W * 4);
  SDL_UpdateTexture(overgrown_b_texture, NULL, overgrown_b.pixel_data, TEXTURE_PIXEL_W * 4);
  SDL_UpdateTexture(water_b_texture, NULL, water_b.pixel_data, TEXTURE_PIXEL_W * 4);
  SDL_UpdateTexture(water_c_texture, NULL, water_c.pixel_data, TEXTURE_PIXEL_W * 4);
  SDL_UpdateTexture(wood_vertical_texture, NULL, wood_vertical.pixel_data, TEXTURE_PIXEL_W * 4);

  return 0;
}

static void player_init(void)
{
  player.rect.x = 72.0f;
  player.rect.y = 72.0f;
  player.rect.w = PLAYER_W;
  player.rect.h = PLAYER_H;
  player.angle = 0.0f;

  Radians radians = convert_deg_to_rads(player.angle);
  player.delta.x = cos(radians) * PLAYER_MOTION_DELTA_MULTIPLIER;
  player.delta.y = sin(radians) * PLAYER_MOTION_DELTA_MULTIPLIER;
}

static void create_2D_line_from_start_point(Line_2D *out_line, Degrees degrees, float length)
{
  Radians radians = convert_deg_to_rads(degrees);
  out_line->end.x = out_line->start.x + length * cos(radians);
  out_line->end.y = out_line->start.y + length * sin(radians);
}

static void draw_player_direction(void)
{
  float length = 30.0f;
  Line_2D line = {
      .start.x = player.rect.x + PLAYER_W / 2,
      .start.y = player.rect.y + PLAYER_H / 2,
  };
  create_2D_line_from_start_point(&line, player.angle, length);
  SDL_RenderLine(renderer, line.start.x, line.start.y, line.end.x, line.end.y);
}

static void draw_player_rect(void)
{
  SDL_RenderRect(renderer, &player.rect);
}

static void cast_rays_from_player(void)
{
  Degrees start_angle = player.angle - PLAYER_FOV_DEG / 2;
  Degrees end_angle = player.angle + PLAYER_FOV_DEG / 2;
  Uint8 r = 255, g = 0, b = 0, a = 255;

  for (Degrees current_angle = start_angle; current_angle <= end_angle; current_angle += PLAYER_FOV_DEG_INC)
  {
    Radians radians = convert_deg_to_rads(current_angle);
    Radians theta = convert_deg_to_rads(current_angle - player.angle);
    Line_2D ray;

    ray.start.x = player.rect.x + (PLAYER_W / 2);
    ray.start.y = player.rect.y + (PLAYER_H / 2);

    Vector_1D x_direction = cos(radians);
    Vector_1D y_direction = sin(radians);
    Vector_1D step_x = (x_direction >= 0) ? 1 : -1;
    Vector_1D step_y = (y_direction >= 0) ? 1 : -1;
    Vector_1D delta_x = fabs(1.0f / x_direction);
    Vector_1D delta_y = fabs(1.0f / y_direction);
    Grid_Point_1D grid_x = floorf(ray.start.x / GRID_CELL_SIZE);
    Grid_Point_1D grid_y = floorf(ray.start.y / GRID_CELL_SIZE);
    Point_1D world_x_normalized = ray.start.x / GRID_CELL_SIZE;
    Point_1D world_y_normalized = ray.start.y / GRID_CELL_SIZE;

    Vector_1D x_distance_to_next_vertical_cell_edge_normalized = (x_direction < 0)
                                                                     ? (world_x_normalized - grid_x) * delta_x
                                                                     : (grid_x + 1 - world_x_normalized) * delta_x;
    Vector_1D y_distance_to_next_horizontal_cell_edge_normalized = (y_direction < 0)
                                                                       ? (world_y_normalized - grid_y) * delta_y
                                                                       : (grid_y + 1 - world_y_normalized) * delta_y;

    Point_1D world_next_wall_intersection_x;
    Point_1D world_next_wall_intersection_y;
    Wall_Surface surface_hit;
    Wall_Type current_grid_cell_value;
    Jagged_Row *current_grid_row;

    bool is_surface_hit = false;
    while (!is_surface_hit)
    {
      if (x_distance_to_next_vertical_cell_edge_normalized < y_distance_to_next_horizontal_cell_edge_normalized)
      {
        world_next_wall_intersection_x = (x_direction < 0)
                                             ? grid_x * GRID_CELL_SIZE
                                             : (grid_x + 1) * GRID_CELL_SIZE;
        world_next_wall_intersection_y = ray.start.y + (world_next_wall_intersection_x - ray.start.x) * y_direction / x_direction;
        x_distance_to_next_vertical_cell_edge_normalized += delta_x;
        grid_x += step_x;
        surface_hit = WS_VERTICAL;
      }
      else
      {
        world_next_wall_intersection_y = (y_direction < 0)
                                             ? grid_y * GRID_CELL_SIZE
                                             : (grid_y + 1) * GRID_CELL_SIZE;
        world_next_wall_intersection_x = ray.start.x + (world_next_wall_intersection_y - ray.start.y) * x_direction / y_direction;
        y_distance_to_next_horizontal_cell_edge_normalized += delta_y;
        grid_y += step_y;
        surface_hit = WS_HORIZONTAL;
      }

      ray.end.x = world_next_wall_intersection_x;
      ray.end.y = world_next_wall_intersection_y;

      current_grid_row = &wall_grid->rows[grid_y];
      current_grid_cell_value = current_grid_row->elements[grid_x];

      if (current_grid_cell_value != 'z')
      {
        is_surface_hit = 1;
        break;
      }
    }

    // Ray lines
    // SDL_SetRenderDrawColor(renderer, r, g, b, a);
    // SDL_RenderLine(renderer, ray.start.x, ray.start.y, ray.end.x, ray.end.y);

    /*
     * 2.5D Rendering
     */
    Scalar ray_length = sqrt(pow(ray.start.x - ray.end.x, 2) + pow(ray.start.y - ray.end.y, 2));
    Point_1D ray_screen_position_x = ((current_angle - start_angle) / PLAYER_FOV_DEG) * (WINDOW_W / 2) + WINDOW_W / 4;
    Scalar perpendicular_distance = ray_length * cos(theta);
    Scalar wall_vertical_strip_height = (GRID_CELL_SIZE * WINDOW_H) / perpendicular_distance;
    Scalar vertical_strip_width = (WINDOW_W / 2) / ((end_angle - start_angle) / PLAYER_FOV_DEG_INC);

    // Center line vertically
    Scalar wall_vertical_offset = (WINDOW_H - wall_vertical_strip_height) / 2;

    /*
     * DRAW FLOORS
     */
    Scalar floor_start_y = wall_vertical_offset + wall_vertical_strip_height;
    Scalar floor_vertical_strip_height = WINDOW_H - floor_start_y;

    // For each vertical pixel in the floor strip
    for (int screen_y = floor_start_y; screen_y < WINDOW_H; screen_y++)
    {
      // Calculate distance to the point on the floor
      Scalar distance = (WINDOW_H / 2.0f) / (screen_y - WINDOW_H / 2.0f);

      // Scale by the player's height (distance to projection plane)
      distance *= GRID_CELL_SIZE; // Adjust this factor as needed

      Point_1D floor_world_x = (player.rect.x + PLAYER_W / 2) + (x_direction / cos(theta)) * distance;
      Point_1D floor_world_y = (player.rect.y + PLAYER_H / 2) + (y_direction / cos(theta)) * distance;

      // Fix grid position calculation using floorf()
      Grid_Point_1D floor_grid_x = floorf(floor_world_x / GRID_CELL_SIZE);
      Grid_Point_1D floor_grid_y = floorf(floor_world_y / GRID_CELL_SIZE);

      // Calculate texture coordinates
      Point_1D texture_x = (int)(floor_world_x) % TEXTURE_PIXEL_W;
      Point_1D texture_y = (int)(floor_world_y) % TEXTURE_PIXEL_H;

      // Create source and destination rectangles for this pixel
      SDL_FRect src_rect = {
          .x = texture_x,
          .y = texture_y,
          .w = 1,
          .h = 1};

      SDL_FRect dst_rect = {
          .x = ray_screen_position_x,
          .y = screen_y,
          .w = vertical_strip_width,
          .h = 1};

      Uint8 floor_brightness = (Uint8)(255.0f * (1.0f - log10f(1.0f + (9.0f * distance / (64 * 16)))));
      // Calculate brightness based on distance (similar to walls)
      SDL_SetTextureColorMod(wood_vertical_texture, floor_brightness, floor_brightness, floor_brightness);
      // SDL_SetTextureColorMod(lava_a_texture, floor_brightness, floor_brightness, floor_brightness);
      // SDL_SetTextureColorMod(lava_b_texture, floor_brightness, floor_brightness, floor_brightness);

      // SDL_RenderTexture(renderer, wood_vertical_texture, &src_rect, &dst_rect);
      switch (floor_grid->rows[floor_grid_y].elements[floor_grid_x])
      {
      case 'A':
      {
        // Render the floor pixel
        SDL_RenderTexture(renderer, wood_vertical_texture, &src_rect, &dst_rect);
        break;
      }
      case 'B':
      {
        // Render the floor pixel
        SDL_RenderTexture(renderer, lava_a_texture, &src_rect, &dst_rect);
        break;
      }
      case 'C':
      {
        // Render the floor pixel
        SDL_RenderTexture(renderer, lava_b_texture, &src_rect, &dst_rect);
        break;
      }
      case 'D':
      {
        // Render the floor pixel
        SDL_RenderTexture(renderer, water_b_texture, &src_rect, &dst_rect);
        break;
      }
      case 'E':
      {
        // Render the floor pixel
        SDL_RenderTexture(renderer, water_c_texture, &src_rect, &dst_rect);
        break;
      }
      case 'F':
      {
        // Render the floor pixel
        SDL_RenderTexture(renderer, lava_c_texture, &src_rect, &dst_rect);
        break;
      }
      default:
      {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, &dst_rect);
      }
      }
    }

    /*
     *
     */

    /*
     * DRAW WALLS
     */

    SDL_FRect wall_rect = {
        .x = ray_screen_position_x,
        .y = wall_vertical_offset,
        .w = vertical_strip_width,
        .h = wall_vertical_strip_height,
    };

    Point_1D wall_x;
    Point_1D texture_x;
    if (surface_hit == WS_VERTICAL)
    {
      wall_x = world_next_wall_intersection_y;
      Point_1D wall_x_normalized = wall_x / GRID_CELL_SIZE;
      Point_1D wall_x_offset_normalized = wall_x_normalized - floorf(wall_x_normalized);
      texture_x = roundf(wall_x_offset_normalized * TEXTURE_PIXEL_W);
    }
    else
    {
      wall_x = world_next_wall_intersection_x;
      Point_1D wall_x_normalized = wall_x / GRID_CELL_SIZE;
      Point_1D wall_x_offset_normalized = wall_x_normalized - floorf(wall_x_normalized);
      texture_x = roundf(wall_x_offset_normalized * TEXTURE_PIXEL_W);
    }

    // Brightness transformations
    Uint8 brightness = (Uint8)(255.0f * (1.0f - log10f(1.0f + (9.0f * perpendicular_distance / (64 * 16)))));
    SDL_SetTextureColorMod(brick_a_texture, brightness, brightness, brightness);
    SDL_SetTextureColorMod(brick_b_texture, brightness, brightness, brightness);
    SDL_SetTextureColorMod(brick_c_texture, brightness, brightness, brightness);
    SDL_SetTextureColorMod(brick_d_texture, brightness, brightness, brightness);
    SDL_SetTextureColorMod(mud_brick_a_texture, brightness, brightness, brightness);
    SDL_SetTextureColorMod(mud_brick_b_texture, brightness, brightness, brightness);
    SDL_SetTextureColorMod(mud_brick_c_texture, brightness, brightness, brightness);
    SDL_SetTextureColorMod(overgrown_a_texture, brightness, brightness, brightness);
    SDL_SetTextureColorMod(overgrown_b_texture, brightness, brightness, brightness);

    switch (current_grid_cell_value)
    {
    case 'A':
    {
      SDL_FRect src_rect = {
          .x = texture_x,
          .y = 0,
          .w = 1,
          .h = TEXTURE_PIXEL_H};
      SDL_RenderTexture(renderer, brick_a_texture, &src_rect, &wall_rect);
      break;
    }
    case 'B':
    {
      SDL_FRect src_rect = {
          .x = texture_x,
          .y = 0,
          .w = 1,
          .h = TEXTURE_PIXEL_H};
      SDL_RenderTexture(renderer, overgrown_a_texture, &src_rect, &wall_rect);
      break;
    }
    case 'C':
    {
      SDL_FRect src_rect = {
          .x = texture_x,
          .y = 0,
          .w = 1,
          .h = TEXTURE_PIXEL_H};
      SDL_RenderTexture(renderer, brick_c_texture, &src_rect, &wall_rect);
      break;
    }
    case 'D':
    {
      SDL_FRect src_rect = {
          .x = texture_x,
          .y = 0,
          .w = 1,
          .h = TEXTURE_PIXEL_H};
      SDL_RenderTexture(renderer, overgrown_b_texture, &src_rect, &wall_rect);
      break;
    }
    case 'E':
    {
      SDL_FRect src_rect = {
          .x = texture_x,
          .y = 0,
          .w = 1,
          .h = TEXTURE_PIXEL_H};
      SDL_RenderTexture(renderer, mud_brick_a_texture, &src_rect, &wall_rect);
      break;
    }
    case 'F':
    {
      SDL_FRect src_rect = {
          .x = texture_x,
          .y = 0,
          .w = 1,
          .h = TEXTURE_PIXEL_H};
      SDL_RenderTexture(renderer, mud_brick_b_texture, &src_rect, &wall_rect);
      break;
    }
    case 'G':
    {
      SDL_FRect src_rect = {
          .x = texture_x,
          .y = 0,
          .w = 1,
          .h = TEXTURE_PIXEL_H};
      SDL_RenderTexture(renderer, mud_brick_c_texture, &src_rect, &wall_rect);
      break;
    }
    default:
    {
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
      SDL_RenderFillRect(renderer, &wall_rect);
    }
    }
  }
}

void draw_player(void)
{
  SDL_SetRenderDrawColor(renderer, 100, 0, 255, 255);
  draw_player_rect();
  draw_player_direction();
}

static void draw_jagged_grid(void)
{
  for (int i = 0; i < wall_grid->num_rows; i++)
  {

    Jagged_Row *current_row = &wall_grid->rows[i];
    bool initialized = false;
    SDL_FRect black_rects[current_row->length];
    SDL_FRect white_rects[current_row->length];
    int black_count = 0;
    int white_count = 0;
    float offset = 0.1f;
    for (int j = 0; j < current_row->length; j++)
    {
      SDL_FRect rect;
      rect.h = GRID_CELL_SIZE * (1.0f - offset);
      rect.w = GRID_CELL_SIZE * (1.0f - offset);
      rect.x = (j * GRID_CELL_SIZE) + (GRID_CELL_SIZE * offset / 2);
      rect.y = (i * GRID_CELL_SIZE) + (GRID_CELL_SIZE * offset / 2);
      if (wall_grid->rows[i].elements[j] != 'z')
      {
        black_rects[black_count++] = rect;
      }
      else
      {
        white_rects[white_count++] = rect;
      }
    }
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRects(renderer, white_rects, white_count);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRects(renderer, black_rects, black_count);
  }
}

void rotate_player(Rotation_Type rotation, float delta_time)
{
  player.angle = player.angle + (rotation * PLAYER_ROTATION_STEP * PLAYER_ROTATION_SPEED * delta_time);
  player.angle = (player.angle < 0) ? 360
                 : (player.angle > 360)
                     ? 0
                     : player.angle;
  Radians radians = convert_deg_to_rads(player.angle);
  player.delta.x = cos(radians) * PLAYER_MOTION_DELTA_MULTIPLIER;
  player.delta.y = sin(radians) * PLAYER_MOTION_DELTA_MULTIPLIER;
}

Grid_Hit_Box convert_world_position_to_grid_position(Point_2D *world_point, float offset)
{
  Hit_Box player_hit_box_world = {
      // Top-left
      .tl.x = world_point->x - offset,
      .tl.y = world_point->y - offset,
      // Top-right
      .tr.x = world_point->x + PLAYER_W + offset,
      .tr.y = world_point->y - offset,
      // Bottom-left
      .bl.x = world_point->x - offset,
      .bl.y = world_point->y + PLAYER_H + offset,
      // Bottom-right
      .br.x = world_point->x + PLAYER_W + offset,
      .br.y = world_point->y + PLAYER_H + offset};

  Grid_Hit_Box player_hit_box_grid = {
      // Top-left
      .tl.x = floor(player_hit_box_world.tl.x / GRID_CELL_SIZE),
      .tl.y = floor(player_hit_box_world.tl.y / GRID_CELL_SIZE),
      // Top-right
      .tr.x = floor(player_hit_box_world.tr.x / GRID_CELL_SIZE),
      .tr.y = floor(player_hit_box_world.tr.y / GRID_CELL_SIZE),
      // Bottom-left
      .bl.x = floor(player_hit_box_world.bl.x / GRID_CELL_SIZE),
      .bl.y = floor(player_hit_box_world.bl.y / GRID_CELL_SIZE),
      // Bottom-right
      .br.x = floor(player_hit_box_world.br.x / GRID_CELL_SIZE),
      .br.y = floor(player_hit_box_world.br.y / GRID_CELL_SIZE),
  };

  return player_hit_box_grid;
}

void move_player(float direction, bool is_sprinting, float delta_time)
{
  Point_2D new_pos = {
      .x = player.rect.x + (direction * player.delta.x * (PLAYER_SPEED + (is_sprinting ? SPRINT_SPEED_INCREASE : 0)) * delta_time),
      .y = player.rect.y + (direction * player.delta.y * (PLAYER_SPEED + (is_sprinting ? SPRINT_SPEED_INCREASE : 0)) * delta_time),
  };

  Grid_Hit_Box player_hit_box_grid = convert_world_position_to_grid_position(&new_pos, PLAYER_INTERACTION_DISTANCE);

  Jagged_Row *tl_grid_cell_row = &wall_grid->rows[player_hit_box_grid.tl.y];
  Jagged_Row *tr_grid_cell_row = &wall_grid->rows[player_hit_box_grid.tr.y];
  Jagged_Row *bl_grid_cell_row = &wall_grid->rows[player_hit_box_grid.bl.y];
  Jagged_Row *br_grid_cell_row = &wall_grid->rows[player_hit_box_grid.br.y];

  Wall_Type tl_grid_cell_value = tl_grid_cell_row->elements[player_hit_box_grid.tl.x];
  Wall_Type tr_grid_cell_value = tr_grid_cell_row->elements[player_hit_box_grid.tr.x];
  Wall_Type bl_grid_cell_value = bl_grid_cell_row->elements[player_hit_box_grid.bl.x];
  Wall_Type br_grid_cell_value = br_grid_cell_row->elements[player_hit_box_grid.br.x];

  if (tl_grid_cell_value == 'z' && tr_grid_cell_value == 'z' &&
      bl_grid_cell_value == 'z' && br_grid_cell_value == 'z')
  {
    player.rect.x = new_pos.x;
    player.rect.y = new_pos.y;
  }
}

uint8_t get_kb_arrow_input_state(void)
{
  uint8_t state = 0b0;
  if (keyboard_state[SDL_SCANCODE_UP])
    state |= KEY_UP;
  if (keyboard_state[SDL_SCANCODE_DOWN])
    state |= KEY_DOWN;
  if (keyboard_state[SDL_SCANCODE_LEFT])
    state |= KEY_LEFT;
  if (keyboard_state[SDL_SCANCODE_RIGHT])
    state |= KEY_RIGHT;
  return state;
}

void handle_player_movement(float delta_time)
{
  uint8_t arrows_state = get_kb_arrow_input_state();
  bool is_sprinting = false;
  if (keyboard_state[SDL_SCANCODE_LSHIFT] || keyboard_state[SDL_SCANCODE_RSHIFT])
  {
    is_sprinting = true;
  }
  if (arrows_state & KEY_LEFT)
  {
    rotate_player(ANTI_CLOCKWISE, delta_time);
  }
  if (arrows_state & KEY_RIGHT)
  {
    rotate_player(CLOCKWISE, delta_time);
  }
  if (arrows_state & KEY_UP)
  {
    move_player(FORWARDS, is_sprinting, delta_time);
  }
  if (arrows_state & KEY_DOWN)
  {
    move_player(BACKWARDS, is_sprinting, delta_time);
  }
}

void update_display(void)
{
  SDL_SetRenderDrawColor(renderer, 30, 0, 30, 255); // White background
  SDL_RenderClear(renderer);
  // draw_jagged_grid();
  // draw_player();
  cast_rays_from_player();
  SDL_RenderPresent(renderer);
}

void run_game_loop(void)
{
  bool loopShouldStop = false;
  uint64_t previous_time = SDL_GetTicks();

  while (!loopShouldStop)
  {
    uint64_t current_time = SDL_GetTicks();
    float delta_time = (current_time - previous_time) / 1000.0f; // Convert to seconds
    previous_time = current_time;

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
      if (event.type == SDL_EVENT_QUIT)
      {
        loopShouldStop = true;
      }
    }

    handle_player_movement(delta_time);
    update_display();
  }
}

int main(int argc, char *argv[])
{
  sdl_init();

  wall_grid = read_grid_csv_file("./assets/levels/level-1-wall.csv");
  floor_grid = read_grid_csv_file("./assets/levels/level-1-floor.csv");
  // print_jagged_grid(floor_grid);
  // print_jagged_grid(wall_grid);

  brick_texture_init();
  // leaves_texture_init();
  // flowers_texture_init();
  // font_init();
  player_init();
  keyboard_state = SDL_GetKeyboardState(NULL);
  run_game_loop();

  free_jagged_grid(wall_grid);

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(win);

  // TTF_CloseFont(font);
  TTF_Quit();

  SDL_Quit();

  return 0;
}