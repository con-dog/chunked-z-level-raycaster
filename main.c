#define CHUNK_X 16
#define CHUNK_Y 16
#define CHUNK_Z 16
#define WALL_HASH_SIZE 1024
#define ANGLE_TO_LUT_INDEX (1.0f / PLAYER_HOZ_FOV_DEG_STEP)
#define LUT_SIZE ((int)(360.0 / PLAYER_HOZ_FOV_DEG_STEP))

#include <time.h>

#include "main.h"

typedef struct Wall
{
  uint16_t coord;      // coords in chunk - [4 flags/unused][4 bits x][4 bits y][4 bits z]
  uint16_t texture_id; // Texture ID -> Value of 0 means this isn't a wall, its empty
} Wall;

typedef struct Chunk
{
  uint16_t coord;             // coords in world - [8 flags/unused][8 bits x][8 bits y][8 bits z]
  Wall walls[WALL_HASH_SIZE]; // Flat Array of entries, use hash_value as index into walls array
  size_t length;              // Number of non-empty walls
} Chunk;

typedef struct World
{
  struct Chunk *chunks; // Flat Array of chunks, only chunks with walls
  size_t length;        // Number of chunks
} World;

/* ******************
 * GLOBALS (START)
 ****************** */
SDL_Window *window;
SDL_Renderer *renderer;
World_Objects_Container *world_objects_container;
Jagged_Grid *floor_grid;
Jagged_Grid *wall_grid;
Player player;
SDL_Texture *rod;
const bool *keyboard_state;
float cos_lut[LUT_SIZE];
float sin_lut[LUT_SIZE];

// This is a walled area 4 z - levels high, 16x by 16y
uint16_t map_chunk[CHUNK_X][CHUNK_Y] = { // 0x000F means z [0-3] has walls, z [3-15] has no walls
    {0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F},
    {0x000F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000F},
    {0x000F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000F},
    {0x000F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000F},
    {0x000F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000F},
    {0x000F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000F},
    {0x000F, 0x0001, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000F},
    {0x000F, 0x0002, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000F},
    {0x000F, 0x0004, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000F},
    {0x000F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000F},
    {0x000F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000F},
    {0x000F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000F},
    {0x000F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000F},
    {0x000F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000F},
    {0x000F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000F},
    {0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F}};

/* ******************
 * GLOBALS (END)
 ****************** */

void do_initialize_trig_lut(void)
{
  for (size_t i = 0; i < LUT_SIZE; i++)
  {
    Radians angle = (i * PLAYER_HOZ_FOV_DEG_STEP) * (M_PI / 180.f);
    cos_lut[i] = cosf(angle);
    sin_lut[i] = sinf(angle);
  }
}

int get_lut_index(Degrees angle)
{
  int index = fmodf(angle * ANGLE_TO_LUT_INDEX, LUT_SIZE);
  return index < 0
             ? index + LUT_SIZE
             : index;
}

uint16_t do_hash_coords(uint8_t x, uint8_t y, uint8_t z)
{
  uint32_t hash = (x << 8) | (y << 4) | z;
  // Multiply by large prime
  hash *= 2654435761u;
  // Take bits 16-25 for better distribution
  return (hash >> 16) & 0x3FF;
}

bool do_hash_insert(Chunk *chunk, Wall *wall, uint16_t index)
{
  uint16_t start_idx = index;
  while (chunk->walls[index].texture_id != 0)
  {
    index = (index + 1) & 0x3FF;
    if (index == start_idx)
    {
      return false; // TABLE FULL
    }
  }
  chunk->walls[index] = *wall;
  return true;
}

Wall *get_wall(Chunk *chunk, uint8_t x, uint8_t y, uint8_t z)
{
  uint16_t idx = do_hash_coords(x, y, z);
  uint16_t start_idx = idx;
  uint16_t coord = (x << 8) | (y << 4) | z;
  while (chunk->walls[idx].texture_id != 0)
  {
    if ((chunk->walls[idx].coord & 0x0FFF) == coord)
    {
      return &chunk->walls[idx];
    }
    idx = (idx + 1) & 0x3FF;
    if (idx == start_idx)
      return NULL; // Not found
  }
  return NULL;
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

static void draw_chunk_level(Chunk *chunk, uint8_t z_level)
{
  if (chunk == NULL)
  {
    exit(1);
  }
  SDL_FRect white_rects[CHUNK_X * CHUNK_Y];
  SDL_FRect black_rects[CHUNK_X * CHUNK_Y];
  SDL_FRect red_rects[CHUNK_X * CHUNK_Y];
  SDL_FRect green_rects[CHUNK_X * CHUNK_Y];
  SDL_FRect blue_rects[CHUNK_X * CHUNK_Y];

  int w_count = 0;
  int blk_count = 0;
  int r_count = 0;
  int g_count = 0;
  int b_count = 0;

  for (uint8_t x = 0; x < CHUNK_X; x++)
  {
    for (uint8_t y = 0; y < CHUNK_Y; y++)
    {
      SDL_FRect rect;
      rect.h = WORLD_CELL_SIZE;
      rect.w = WORLD_CELL_SIZE;
      rect.x = (x * WORLD_CELL_SIZE);
      rect.y = (y * WORLD_CELL_SIZE);
      Wall *wall = get_wall(chunk, x, y, z_level);
      if (wall == NULL)
      {
        white_rects[w_count++] = rect;
        continue;
      }
      if (get_wall(chunk, x, y, z_level)->texture_id == 1)
      {
        black_rects[blk_count++] = rect;
      }
      else if (get_wall(chunk, x, y, z_level)->texture_id == 2)
      {
        red_rects[r_count++] = rect;
      }
      else if (get_wall(chunk, x, y, z_level)->texture_id == 3)
      {
        green_rects[g_count++] = rect;
      }
      else if (get_wall(chunk, x, y, z_level)->texture_id == 4)
      {
        blue_rects[b_count++] = rect;
      }
    }
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRects(renderer, white_rects, w_count);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRects(renderer, black_rects, blk_count);

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRects(renderer, red_rects, r_count);

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRects(renderer, green_rects, g_count);

    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_RenderFillRects(renderer, blue_rects, b_count);
  }
}

static void draw_player_rect(void)
{
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  SDL_RenderRect(renderer, &player.rect);
}

void rotate_player(Rotation_Type rotation, float delta_time)
{
  player.angle = player.angle + (rotation * PLAYER_ROTATION_STEP *
                                 PLAYER_ROTATION_SPEED * delta_time);
  player.angle = (player.angle < 0)     ? 360
                 : (player.angle > 360) ? 0
                                        : player.angle;
  Radians radians = convert_deg_to_rads(player.angle);
  player.delta.x = cos(radians) * PLAYER_MOTION_DELTA_MULTIPLIER;
  player.delta.y = sin(radians) * PLAYER_MOTION_DELTA_MULTIPLIER;
}

void move_player(float direction, bool is_sprinting, float delta_time)
{
  Point_2D new_pos = {
      .x = player.rect.x +
           (direction * player.delta.x *
            (PLAYER_SPEED + (is_sprinting ? SPRINT_SPEED_INCREASE : 0)) *
            delta_time),
      .y = player.rect.y +
           (direction * player.delta.y *
            (PLAYER_SPEED + (is_sprinting ? SPRINT_SPEED_INCREASE : 0)) *
            delta_time),
  };

  player.rect.x = new_pos.x;
  player.rect.y = new_pos.y;
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
  if (keyboard_state[SDL_SCANCODE_LSHIFT] ||
      keyboard_state[SDL_SCANCODE_RSHIFT])
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

bool do_initialize_chunk(Chunk *chunk)
{
  for (uint8_t i = 0; i < CHUNK_X; i++)
  {
    for (uint8_t j = 0; j < CHUNK_Y; j++)
    {
      uint16_t z = map_chunk[i][j];
      for (uint8_t k = 0; k < CHUNK_Z; k++)
      {
        if (z & (1 << k))
        {
          uint8_t flags = 0b0011;
          uint16_t coord = (flags << 12) | (i << 8) | (j << 4) | k;
          int texture_id = 0;
          switch (k)
          {
          case 0:
            texture_id = 1;
            break;
          case 1:
            texture_id = 2;
            break;
          case 2:
            texture_id = 3;
            break;
          case 3:
            texture_id = 4;
            break;
          default:
            texture_id = 0;
          }
          Wall wall = {
              .coord = coord,
              .texture_id = texture_id,
          };
          uint16_t index = do_hash_coords(i, j, k);
          bool result = do_hash_insert(chunk, &wall, index);
        }
      }
    }
  }
  chunk->coord = 0;
  chunk->length = WALL_HASH_SIZE;
  return true;
}

static void draw_player_direction(void)
{
  float length = 30.0f;
  Line_2D line = {
      .start.x = player.rect.x + PLAYER_W / 2,
      .start.y = player.rect.y + PLAYER_H / 2,
  };
  Radians angle = convert_deg_to_rads(player.angle);
  line.end.x = line.start.x + length * cosf(angle);
  line.end.y = line.start.y + length * sinf(angle);

  SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
  SDL_RenderLine(renderer, line.start.x, line.start.y, line.end.x, line.end.y);
}

bool do_initialize_world(Chunk *chunk)
{
  bool result = do_initialize_chunk(chunk);
  return result;
}

float get_player_x_centered(Player *player)
{
  return player->rect.x + player->rect.w / 2;
}

float get_player_y_centered(Player *player)
{
  return player->rect.y + player->rect.h / 2;
}

Scalar calculate_ray_length(Line_2D *ray)
{
  return sqrtf(powf(ray->start.x - ray->end.x, 2) + powf(ray->start.y - ray->end.y, 2));
}

bool is_bit_set(uint16_t mask, uint8_t idx)
{
  return (mask >> idx) & 1;
}

void do_raycasting(Chunk *chunk)
{
  Degrees start_ang = player.angle - PLAYER_HLF_HOZ_FOV_DEG;
  Degrees end_ang = player.angle + PLAYER_HLF_HOZ_FOV_DEG;
  Degrees delta_ang = end_ang - start_ang;

  Point_1D player_x_center = get_player_x_centered(&player);
  Point_1D player_y_center = get_player_y_centered(&player);

  for (Degrees curr_ang = start_ang; curr_ang <= end_ang; curr_ang += PLAYER_HOZ_FOV_DEG_STEP)
  {
    /*
     * Horizontal ray setup
     */

    int ang_lut_idx = get_lut_index(curr_ang);
    int theta_lut_idx = get_lut_index(curr_ang - player.angle);

    Line_2D ray = {
        .start.x = player_x_center,
        .start.y = player_y_center,
    };

    Index map_x_idx = floorf(ray.start.x / WORLD_CELL_SIZE); // x index in map chunk array
    Index map_y_idx = floorf(ray.start.y / WORLD_CELL_SIZE); // y index in map chunk array
    Point_1D nworld_x = ray.start.x / WORLD_CELL_SIZE;       // x point in world normalized
    Point_1D nworld_y = ray.start.y / WORLD_CELL_SIZE;       // y point in world normalized
    Vector_1D x_dirv = cos_lut[ang_lut_idx];                 // x direction vector
    Vector_1D y_dirv = sin_lut[ang_lut_idx];                 // y direction vector
    Vector_1D x_stepv = x_dirv >= 0 ? 1 : -1;                // x-axis step vector
    Vector_1D y_stepv = y_dirv >= 0 ? 1 : -1;                // y-axis step vector
    Vector_1D x_deltav = fabs(1.0f / x_dirv);
    Vector_1D y_deltav = fabs(1.0f / y_dirv);
    Vector_1D nworld_x_edge_dist = x_dirv < 0                                   // Normalized distance to next vertical edge
                                       ? (nworld_x - map_x_idx) * x_deltav      // Facing to the right/east next edge eg: |  *-->  |
                                       : (map_x_idx + 1 - nworld_x) * x_deltav; // Facing to the left/west previous edge eg: |  <--*  |
    Vector_1D nworld_y_edge_dist = y_dirv < 0                                   // Normalized distance to next horizontal edge
                                       ? (nworld_y - map_y_idx) * y_deltav      // Facing south
                                       : (map_y_idx + 1 - nworld_y) * y_deltav; // Facing north

    typedef struct WallBuffer
    {
      Wall *wall; // Wall
      SDL_FRect rect;
    } WallBuffer;

    /*
     * Wall collision logic
     */
    bool is_wall_hit = false;
    Point_2D wall_intxn_point;
    Plane hit_plane;
    WallBuffer wall_buff[4];
    uint16_t ray_zmask = 0xFFF0;
    while (ray_zmask != 0xFFFF) // TODO ! Maybe add some ray distance logic too, so rays don't go forever
    {
      /*
       * DDA axis choice
       */
      if (nworld_x_edge_dist <= nworld_y_edge_dist)
      {
        wall_intxn_point.x = x_dirv < 0
                                 ? map_x_idx * WORLD_CELL_SIZE
                                 : (map_x_idx + 1) * WORLD_CELL_SIZE;
        wall_intxn_point.y = ray.start.y + (wall_intxn_point.x - ray.start.x) * y_dirv / x_dirv;
        nworld_x_edge_dist += x_deltav;
        map_x_idx += x_stepv;
        hit_plane = X_PLANE; // eg: a vertical edge
      }
      else
      {
        wall_intxn_point.y = y_dirv < 0
                                 ? map_y_idx * WORLD_CELL_SIZE
                                 : (map_y_idx + 1) * WORLD_CELL_SIZE;
        wall_intxn_point.x = ray.start.x + (wall_intxn_point.y - ray.start.y) * x_dirv / y_dirv;
        nworld_y_edge_dist += y_deltav;
        map_y_idx += y_stepv;
        hit_plane = Y_PLANE; // eg: a horizontal edge
      }

      ray.end.x = wall_intxn_point.x;
      ray.end.y = wall_intxn_point.y;

      const Scalar ray_length = calculate_ray_length(&ray);
      const Scalar ray_perp_dist = ray_length * cos_lut[theta_lut_idx];

      uint16_t z_lvls = map_chunk[map_x_idx][map_y_idx];
      uint16_t z_max = floorf((32.0f + ray_perp_dist * tanf(convert_deg_to_rads(15))) / WORLD_CELL_SIZE); // eg for z_max = 3;
      uint16_t z_mask = (1 << z_max + 1) - 1;                                                             // z_mask = 0b1111; only check the first 4 levels (from 0) up to z = 3

      if (!(z_lvls & z_mask))
      {
        continue;
      }

      const Scalar wall_w = WINDOW_HLF_W / (delta_ang * PLAYER_HOZ_FOV_DEG_STEP_INV);
      const Scalar x_screen_offset = ((curr_ang - start_ang) * PLAYER_HOZ_FOV_DEG_INV) * WINDOW_HLF_W + WINDOW_QRT_W; // WINDOW_HLF_W + WINDOW_QRT_W center the x coord in the screen
      const Scalar PLAYER_EYE_HEIGHT = 0.5f * WORLD_CELL_SIZE;
      const Scalar VERT_FOV_RAD = convert_deg_to_rads(PLAYER_VERT_FOV_DEG);
      const Scalar VERT_SCALE = (WINDOW_H / VERT_FOV_RAD) * (PLAYER_VERT_FOV_DEG / PLAYER_HOZ_FOV_DEG); // This makes the cubes square etc

      // for each z in z_lvls
      for (uint8_t z = 0; z < CHUNK_Z; z++)
      {
        if (z_lvls & (1 << z))
        {
          Wall *wall = get_wall(chunk, map_x_idx, map_y_idx, z);
          if (wall != NULL && wall->texture_id != 0 && !(is_bit_set(ray_zmask, z)))
          {
            ray_zmask |= (1 << z);

            Scalar wall_bottom_y = z * WORLD_CELL_SIZE;
            Scalar wall_top_y = (z + 1) * WORLD_CELL_SIZE;

            Scalar angle_to_bottom = atan2f(wall_bottom_y - PLAYER_EYE_HEIGHT, ray_perp_dist);
            Scalar angle_to_top = atan2f(wall_top_y - PLAYER_EYE_HEIGHT, ray_perp_dist);

            Scalar screen_y_bottom = WINDOW_HLF_H - (angle_to_bottom * VERT_SCALE);
            Scalar screen_y_top = WINDOW_HLF_H - (angle_to_top * VERT_SCALE);

            SDL_FRect rect = {
                .x = x_screen_offset,
                .y = screen_y_top,
                .w = wall_w,
                .h = screen_y_bottom - screen_y_top};

            wall_buff[z].rect = rect;
            wall_buff[z].wall = wall;
          }
        }
      }
    }

    for (int i = 3; i >= 0; i--)
    {
      WallBuffer wall_b = wall_buff[i];
      if (wall_b.wall != NULL && wall_b.wall->texture_id != 0)
      {
        int texture_id = wall_b.wall->texture_id;
        switch (texture_id)
        {
        case 1:
          SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
          break;
        case 2:
          SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
          break;
        case 3:
          SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
          break;
        case 4:
          SDL_SetRenderDrawColor(renderer, 125, 25, 123, 255);
          break;
        default:
          SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        }
        SDL_RenderFillRect(renderer, &wall_b.rect);
      }
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderLine(renderer, 0, WINDOW_HLF_H, WINDOW_W, WINDOW_HLF_H);
    SDL_RenderLine(renderer, WINDOW_HLF_W, 0, WINDOW_HLF_W, WINDOW_H);
  }
}

void update_display(Chunk *chunk)
{
  SDL_SetRenderDrawColor(renderer, 127, 127, 127, 255);
  SDL_RenderClear(renderer);
  // draw_chunk_level(chunk, 0);
  // draw_player_rect();
  // draw_player_direction();
  do_raycasting(chunk);
  // exit(EXIT_SUCCESS);
  SDL_RenderPresent(renderer);
}

void run_game_loop(Chunk *chunk)
{
  uint32_t frame_count = 0;
  uint32_t fps_last_time = SDL_GetTicks();
  uint32_t current_fps = 0;
  //
  bool loopShouldStop = false;
  uint64_t previous_time = SDL_GetTicks();

  while (!loopShouldStop)
  {
    uint64_t current_time = SDL_GetTicks();
    float delta_time =
        (current_time - previous_time) / 1000.0f; // Convert to seconds
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
    time_t start = clock();
    update_display(chunk);
    frame_count++;
    uint32_t current_time_fps = SDL_GetTicks();
    time_t end = clock();

    if (current_time_fps - fps_last_time >= 1000)
    { // Every second
      current_fps = frame_count;
      frame_count = 0;
      fps_last_time = current_time_fps;
      printf("Current FPS: %u\n", current_fps);
    }

    // printf("Frame time %f\n", (end - start) / CLOCKS_PER_SEC);
  }
}

int main()
{
  const char *title = "2.5D Raycasting Game Engine";
  setup_sdl(title, WINDOW_W, WINDOW_H, SDL_WINDOW_RESIZABLE, &window,
            &renderer);

  do_initialize_trig_lut();
  Chunk chunk = {0};
  do_initialize_world(&chunk);

  player_init();
  keyboard_state = SDL_GetKeyboardState(NULL);
  run_game_loop(&chunk);

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}