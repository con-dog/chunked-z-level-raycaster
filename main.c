#define CHUNK_X 16
#define CHUNK_Y 16
#define CHUNK_Z 16
#define WALL_HASH_SIZE 1024

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

uint16_t do_hash_coords(uint8_t x, uint8_t y, uint8_t z)
{
  // Since x,y,z are 0-15, we can use bit shifts
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
  uint16_t loops = 0;
  uint16_t idx = do_hash_coords(x, y, z);
  uint16_t start_idx = idx;
  uint16_t coord = (x << 8) | (y << 4) | z;
  while (chunk->walls[idx].texture_id != 0)
  {
    if ((chunk->walls[idx].coord & 0x0FFF) == coord)
    {
      return &chunk->walls[idx];
    }
    printf("LOOPING: %d\n", loops);
    idx = (idx + 1) & 0x3FF;
    loops++;
    if (idx == start_idx)
      return NULL; // Not found
  }
  return NULL;
}

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

// This is a walled area 4 z - levels high, 16x by 16y
uint16_t map_chunk[CHUNK_X][CHUNK_Y] = { // 0x000F means z [0-3] has walls, z [3-15] has no walls
    {0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F},
    {0x000F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000F},
    {0x000F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000F},
    {0x000F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000F},
    {0x000F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000F},
    {0x000F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000F},
    {0x000F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000F},
    {0x000F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000F},
    {0x000F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000F},
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
          // Default texture for now, and solid wall and floor
          uint8_t flags = 0b0011;
          uint16_t coord = (flags << 12) | (i << 8) | (j << 4) | k;

          Wall wall = {
              .coord = coord,
              .texture_id = 1,
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

bool do_initialize_world(Chunk *chunk)
{
  bool result = do_initialize_chunk(chunk);
  return result;
}

// void run_game_loop(void)
// {
//   bool loopShouldStop = false;
//   uint64_t previous_time = SDL_GetTicks();

//   while (!loopShouldStop)
//   {
//     uint64_t current_time = SDL_GetTicks();
//     float delta_time =
//         (current_time - previous_time) / 1000.0f; // Convert to seconds
//     previous_time = current_time;

//     SDL_Event event;
//     while (SDL_PollEvent(&event))
//     {
//       if (event.type == SDL_EVENT_QUIT)
//       {
//         loopShouldStop = true;
//       }
//     }

//     update_display();
//   }
// }

int main()
{
  const char *title = "2.5D Raycasting Game Engine";
  setup_sdl(title, WINDOW_W, WINDOW_H, SDL_WINDOW_RESIZABLE, &window,
            &renderer);

  Chunk chunk = {0};
  do_initialize_world(&chunk);
  Wall *wall = get_wall(&chunk, 15, 15, 3);
  if (wall == NULL)
  {
    printf("No wall found\n");
  }
  else
  {
    printf("WALL texture id: %d", wall->texture_id);
  }

  // player_init();
  keyboard_state = SDL_GetKeyboardState(NULL);
  // run_game_loop();

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}