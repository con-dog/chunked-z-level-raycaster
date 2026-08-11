#define CHUNK_X 16
#define CHUNK_Y 16
#define CHUNK_Z 16
#define WALL_HASH_SIZE 1024
#define ANGLE_TO_LUT_INDEX (1.0f / PLAYER_HOZ_FOV_DEG_STEP)
#define LUT_SIZE 960
#define MAX_WALL_SPANS_PER_RAY ((CHUNK_X + CHUNK_Y) * CHUNK_Z)
#define SDL_MAIN_USE_CALLBACKS 1

#include "main.h"

typedef struct Wall
{
  uint16_t coord;      // coords in chunk - [4 flags/unused][4 bits x][4 bits y][4 bits z]
  uint16_t texture_id; // Texture ID -> Value of 0 means this isn't a wall, its empty
} Wall;

typedef struct WallContainer
{
  Wall wall;
  SDL_FRect rect;
} WallContainer;

typedef struct Chunk
{
  uint16_t coord;             // coords in world - [8 flags/unused][8 bits x][8 bits y][8 bits z]
  Wall walls[WALL_HASH_SIZE]; // Flat Array of entries, use hash_value as index into walls array
  size_t length;              // Number of non-empty walls
} Chunk;

typedef struct AppState
{
  Chunk chunk;
  uint64_t previous_time;
} AppState;

/* ******************
 * GLOBALS (START)
 ****************** */
SDL_Window *window;
SDL_Renderer *renderer;
Player player;
const bool *keyboard_state;
float cos_lut[LUT_SIZE];
float sin_lut[LUT_SIZE];

// Each cell is a z-level bitmask: bit n set = wall at z level n.
// 0x0000 open floor          0x000F full wall (z 0-3)
// 0x0001/0x0003/0x0007 steps 0x00FF tall wall (z 0-7)
// 0xFFFF tower (z 0-15)      0x000C floating arch (open z 0-1, solid z 2-3)
// Layout: spawn room (top-left) with two arched doorways, step gradients,
// a central plaza with a 16-high tower, tall-walled corridors, and a
// second tower courtyard to the south-east.
uint16_t map_chunk[CHUNK_X][CHUNK_Y] = {
    {0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F},
    {0x000F, 0x0000, 0x0000, 0x0000, 0x0000, 0x000F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000F},
    {0x000F, 0x0000, 0x0000, 0x0000, 0x0000, 0x000F, 0x0000, 0x0001, 0x0003, 0x0007, 0x000F, 0x0000, 0x00FF, 0x00FF, 0x0000, 0x000F},
    {0x000F, 0x0000, 0x0000, 0x0000, 0x0000, 0x000C, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x00FF, 0x0000, 0x0000, 0x000F},
    {0x000F, 0x0000, 0x0000, 0x0000, 0x0000, 0x000F, 0x0000, 0x0007, 0x0000, 0x0000, 0x000F, 0x0000, 0x00FF, 0x0000, 0x0000, 0x000F},
    {0x000F, 0x000F, 0x000C, 0x000F, 0x000F, 0x000F, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x000F, 0x0000, 0x00FF, 0x0000, 0x0000, 0x000F},
    {0x000F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0003, 0x0000, 0x0000, 0x000F, 0x0000, 0x0000, 0x0000, 0x0000, 0x000F},
    {0x000F, 0x0000, 0x0001, 0x0000, 0x0000, 0x000F, 0x0000, 0x0000, 0x0000, 0x0000, 0x000C, 0x0000, 0x000F, 0x000F, 0x0000, 0x000F},
    {0x000F, 0x0000, 0x0003, 0x0000, 0x0000, 0x000F, 0x000F, 0x000F, 0x000C, 0x000F, 0x000F, 0x0000, 0x000F, 0x0000, 0x0000, 0x000F},
    {0x000F, 0x0000, 0x0007, 0x0000, 0x0000, 0x0000, 0x0000, 0x000F, 0x0000, 0x0000, 0x0000, 0x0000, 0x000F, 0x0000, 0x0007, 0x000F},
    {0x000F, 0x0000, 0x000F, 0x0000, 0x0000, 0x0000, 0x0000, 0x000F, 0x0000, 0x0001, 0x0000, 0x0000, 0x000C, 0x0000, 0x0003, 0x000F},
    {0x000F, 0x0000, 0x0000, 0x0000, 0x0000, 0x000F, 0x0000, 0x0000, 0x0000, 0x0003, 0x0000, 0x0000, 0x000F, 0x0000, 0x0001, 0x000F},
    {0x000F, 0x0000, 0x00FF, 0x00FF, 0x0000, 0x000F, 0x0000, 0x0000, 0x0000, 0x0007, 0x0000, 0x0000, 0x000F, 0x000F, 0x0000, 0x000F},
    {0x000F, 0x0000, 0x0000, 0x00FF, 0x0000, 0x000F, 0x0000, 0xFFFF, 0x0000, 0x000F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000F},
    {0x000F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000F},
    {0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0xFFFF}};

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
          int texture_id = k + 1;
          Wall wall = {
              .coord = coord,
              .texture_id = texture_id,
          };
          uint16_t index = do_hash_coords(i, j, k);
          do_hash_insert(chunk, &wall, index);
        }
      }
    }
  }
  chunk->coord = 0;
  chunk->length = WALL_HASH_SIZE;
  return true;
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
  const Scalar dx = ray->start.x - ray->end.x;
  const Scalar dy = ray->start.y - ray->end.y;
  return sqrtf(dx * dx + dy * dy);
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
  const Scalar wall_w = WINDOW_HLF_W / (delta_ang * PLAYER_HOZ_FOV_DEG_STEP_INV);
  const Scalar vert_scale = ((WINDOW_H / 2.0f) / tanf(PLAYER_VERT_FOV_RAD / 2.0f)) * WINDOW_H / WINDOW_W;

  for (Degrees curr_ang = start_ang; curr_ang <= end_ang; curr_ang += PLAYER_HOZ_FOV_DEG_STEP)
  // for (int i = 0; i < 1; i++)
  {
    /*
     * Horizontal ray setup
     */
    // Degrees curr_ang = player.angle;

    int ang_lut_idx = get_lut_index(curr_ang);
    int theta_lut_idx = get_lut_index(curr_ang - player.angle);

    Line_2D ray = {
        .start.x = player_x_center,
        .start.y = player_y_center,
    };

    Index map_x_idx = floorf(ray.start.x * WORLD_CELL_SIZE_INV); // x index in map chunk array
    Index map_y_idx = floorf(ray.start.y * WORLD_CELL_SIZE_INV); // y index in map chunk array
    Point_1D nworld_x = ray.start.x * WORLD_CELL_SIZE_INV;       // x point in world normalized
    Point_1D nworld_y = ray.start.y * WORLD_CELL_SIZE_INV;       // y point in world normalized
    Vector_1D x_dirv = cos_lut[ang_lut_idx];                     // x direction vector
    Vector_1D y_dirv = sin_lut[ang_lut_idx];                     // y direction vector
    int x_stepv = x_dirv >= 0 ? 1 : -1;                          // x-axis step vector
    int y_stepv = y_dirv >= 0 ? 1 : -1;                          // y-axis step vector
    Vector_1D x_deltav = fabsf(1.0f / x_dirv);
    Vector_1D y_deltav = fabsf(1.0f / y_dirv);
    Vector_1D nworld_x_edge_dist = x_dirv < 0                                   // Normalized distance to next vertical edge
                                       ? (nworld_x - map_x_idx) * x_deltav      // Facing to the right/east next edge eg: |  *-->  |
                                       : (map_x_idx + 1 - nworld_x) * x_deltav; // Facing to the left/west previous edge eg: |  <--*  |
    Vector_1D nworld_y_edge_dist = y_dirv < 0                                   // Normalized distance to next horizontal edge
                                       ? (nworld_y - map_y_idx) * y_deltav      // Facing south
                                       : (map_y_idx + 1 - nworld_y) * y_deltav; // Facing north

    /*
     * Wall collision logic
     */
    Point_2D wall_intxn_point;
    WallContainer wall_spans[MAX_WALL_SPANS_PER_RAY];
    size_t wall_span_count = 0;

    // has this y-range aready been drawn to? If so, skip/occlude (front to back)
    // && (ray_zmask != 0xFFFF) is another condition to check
    // Prevent ray going outside chunk
    while (map_x_idx < CHUNK_X && map_x_idx >= 0 && map_y_idx < CHUNK_Y && map_y_idx >= 0)
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
      }
      else
      {
        wall_intxn_point.y = y_dirv < 0
                                 ? map_y_idx * WORLD_CELL_SIZE
                                 : (map_y_idx + 1) * WORLD_CELL_SIZE;
        wall_intxn_point.x = ray.start.x + (wall_intxn_point.y - ray.start.y) * x_dirv / y_dirv;
        nworld_y_edge_dist += y_deltav;
        map_y_idx += y_stepv;
      }

      if (map_x_idx < 0 || map_x_idx >= CHUNK_X || map_y_idx < 0 || map_y_idx >= CHUNK_Y)
      {
        break;
      }

      ray.end.x = wall_intxn_point.x;
      ray.end.y = wall_intxn_point.y;

      const Scalar ray_length = calculate_ray_length(&ray);
      const Scalar ray_perp_dist = ray_length * cos_lut[theta_lut_idx];
      const Scalar PLAYER_EYE_HEIGHT = 0.5f * WORLD_CELL_SIZE;

      float max_height = PLAYER_EYE_HEIGHT + ray_perp_dist * tanf((PLAYER_VERT_FOV_RAD / 2.0f) * WINDOW_W / WINDOW_H);
      int z_max = (int)ceilf(max_height * WORLD_CELL_SIZE_INV);
      if (z_max >= CHUNK_Z)
      {
        z_max = CHUNK_Z - 1;
      }

      uint16_t z_lvls = map_chunk[map_x_idx][map_y_idx];
      const Scalar x_screen_offset = ((curr_ang - start_ang) * PLAYER_HOZ_FOV_DEG_INV) * WINDOW_HLF_W + WINDOW_QRT_W; // WINDOW_HLF_W + WINDOW_QRT_W center the x coord in the screen

      for (int z = 0; z <= z_max; z++)
      {
        if (z_lvls & (1 << z))
        {
          Wall *wall = get_wall(chunk, map_x_idx, map_y_idx, z);
          if (wall != NULL && wall->texture_id != 0)
          {
            Scalar wall_bottom_y = z * WORLD_CELL_SIZE;
            Scalar wall_top_y = (z + 1) * WORLD_CELL_SIZE;

            Scalar angle_to_bottom = atan2f(wall_bottom_y - PLAYER_EYE_HEIGHT, ray_perp_dist);
            Scalar angle_to_top = atan2f(wall_top_y - PLAYER_EYE_HEIGHT, ray_perp_dist);

            Scalar screen_y_bottom = WINDOW_HLF_H - (angle_to_bottom * vert_scale);
            Scalar screen_y_top = WINDOW_HLF_H - (angle_to_top * vert_scale);

            SDL_FRect rect = {
                .x = x_screen_offset,
                .y = screen_y_top,
                .w = wall_w,
                .h = screen_y_bottom - screen_y_top};

            if (wall_span_count < MAX_WALL_SPANS_PER_RAY)
            {
              wall_spans[wall_span_count++] = (WallContainer){
                  .rect = rect,
                  .wall = *wall,
              };
            }
          }
        }
      }
    }

    while (wall_span_count > 0)
    {
      WallContainer *wall_span = &wall_spans[--wall_span_count];
      int texture_id = wall_span->wall.texture_id;
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
      case 5:
        SDL_SetRenderDrawColor(renderer, 200, 55, 220, 255);
        break;
      default:
        SDL_SetRenderDrawColor(renderer, 30, 50, 80, 255);
      }
      SDL_RenderRect(renderer, &wall_span->rect);
    }
  }
}

void update_display(Chunk *chunk)
{
  SDL_SetRenderDrawColor(renderer, 127, 127, 127, 255);
  SDL_RenderClear(renderer);
  do_raycasting(chunk);
  SDL_RenderPresent(renderer);
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv)
{
  (void)argc;
  (void)argv;

  AppState *state = calloc(1, sizeof(*state));
  if (state == NULL)
  {
    return SDL_APP_FAILURE;
  }
  *appstate = state;

  const char *title = "Chunked Z-Level Raycaster — Browser Demo";
  if (setup_sdl(title, WINDOW_W, WINDOW_H, SDL_WINDOW_RESIZABLE, &window,
                &renderer) != 0)
  {
    return SDL_APP_FAILURE;
  }

  do_initialize_trig_lut();
  do_initialize_world(&state->chunk);

  player_init();
  keyboard_state = SDL_GetKeyboardState(NULL);
  state->previous_time = SDL_GetTicks();

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
  (void)appstate;
  return event->type == SDL_EVENT_QUIT ? SDL_APP_SUCCESS : SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
  AppState *state = appstate;
  uint64_t current_time = SDL_GetTicks();
  float delta_time = (current_time - state->previous_time) / 1000.0f;
  state->previous_time = current_time;
  if (delta_time > 0.1f)
  {
    delta_time = 0.1f;
  }

  handle_player_movement(delta_time);
  update_display(&state->chunk);

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
  (void)result;

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  free(appstate);
}
