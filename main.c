#include "main.h"

/* ******************
 * GLOBALS (START)
 ****************** */
SDL_Window   *window;
SDL_Renderer *renderer;

World_Objects_Container *world_objects_container;

Jagged_Grid *floor_grid;
Jagged_Grid *wall_grid;

Player player;

const bool *keyboard_state;

/* ******************
 * GLOBALS (END)
 ****************** */

static void player_init(void) {
  player.rect.x = 72.0f;
  player.rect.y = 72.0f;
  player.rect.w = PLAYER_W;
  player.rect.h = PLAYER_H;
  player.angle  = 0.0f;

  Radians radians = convert_deg_to_rads(player.angle);
  player.delta.x  = cos(radians) * PLAYER_MOTION_DELTA_MULTIPLIER;
  player.delta.y  = sin(radians) * PLAYER_MOTION_DELTA_MULTIPLIER;
}

static void create_2D_line_from_start_point(Line_2D *out_line, Degrees degrees,
                                            float length) {
  Radians radians = convert_deg_to_rads(degrees);
  out_line->end.x = out_line->start.x + length * cos(radians);
  out_line->end.y = out_line->start.y + length * sin(radians);
}

static void draw_player_direction(void) {
  float   length = 30.0f;
  Line_2D line   = {
        .start.x = player.rect.x + PLAYER_W / 2,
        .start.y = player.rect.y + PLAYER_H / 2,
  };
  create_2D_line_from_start_point(&line, player.angle, length);
  SDL_RenderLine(renderer, line.start.x, line.start.y, line.end.x, line.end.y);
}

static void draw_player_rect(void) { SDL_RenderRect(renderer, &player.rect); }

static Scalar calculate_ray_perpendicular_distance(Line_2D *ray,
                                                   Radians  theta) {
  Scalar ray_length = sqrt(pow(ray->start.x - ray->end.x, 2) +
                           pow(ray->start.y - ray->end.y, 2));
  return ray_length * cos(theta);
}

static void cast_rays_from_player(void) {
  Degrees start_angle = player.angle - PLAYER_FOV_DEG / 2;
  Degrees end_angle   = player.angle + PLAYER_FOV_DEG / 2;

  for (Degrees curr_angle_deg = start_angle; curr_angle_deg <= end_angle;
       curr_angle_deg += PLAYER_FOV_DEG_INC) {
    /*
     * Ray Setup logic
     */
    Line_2D     ray;
    Jagged_Row *curr_wall_grid_row;

    Radians curr_angle_rads = convert_deg_to_rads(curr_angle_deg);
    Radians theta = convert_deg_to_rads(curr_angle_deg - player.angle);

    ray.start.x = player.rect.x + (PLAYER_W / 2);
    ray.start.y = player.rect.y + (PLAYER_H / 2);

    IPoint_1D  grid_x                = floorf(ray.start.x / GRID_CELL_SIZE);
    Point_1D   norm_x                = ray.start.x / GRID_CELL_SIZE;
    Vector_1D  x_dir                 = cos(curr_angle_rads);
    IVector_1D step_x                = (x_dir >= 0) ? 1 : -1;
    Vector_1D  delta_x               = fabs(1.0f / x_dir);
    Vector_1D  norm_x_dist_cell_edge = (x_dir < 0)
                                           ? (norm_x - grid_x) * delta_x
                                           : (grid_x + 1 - norm_x) * delta_x;

    IPoint_1D  grid_y                = floorf(ray.start.y / GRID_CELL_SIZE);
    Point_1D   norm_y                = ray.start.y / GRID_CELL_SIZE;
    Vector_1D  y_dir                 = sin(curr_angle_rads);
    IVector_1D step_y                = (y_dir >= 0) ? 1 : -1;
    Vector_1D  delta_y               = fabs(1.0f / y_dir);
    Vector_1D  norm_y_dist_cell_edge = (y_dir < 0)
                                           ? (norm_y - grid_y) * delta_y
                                           : (grid_y + 1 - norm_y) * delta_y;

    Point_1D     wall_x_intersection;
    Point_1D     wall_y_intersection;
    Wall_Surface surface_hit;

    /*
     * Wall collision and step logic
     */
    bool is_wall_hit = false;
    while (!is_wall_hit) {
      if (norm_x_dist_cell_edge < norm_y_dist_cell_edge) {
        wall_x_intersection = (x_dir < 0) ? grid_x * GRID_CELL_SIZE
                                          : (grid_x + 1) * GRID_CELL_SIZE;
        wall_y_intersection =
            ray.start.y + (wall_x_intersection - ray.start.x) * y_dir / x_dir;
        norm_x_dist_cell_edge += delta_x;
        grid_x += step_x;
        surface_hit = WS_VERTICAL;
      } else {
        wall_y_intersection = (y_dir < 0) ? grid_y * GRID_CELL_SIZE
                                          : (grid_y + 1) * GRID_CELL_SIZE;
        wall_x_intersection =
            ray.start.x + (wall_y_intersection - ray.start.y) * x_dir / y_dir;
        norm_y_dist_cell_edge += delta_y;
        grid_y += step_y;
        surface_hit = WS_HORIZONTAL;
      }

      ray.end.x = wall_x_intersection;
      ray.end.y = wall_y_intersection;

      /*
       * Collision check for non EMPTY cell
       */
      curr_wall_grid_row = &wall_grid->rows[grid_y];
      // Ocurr_grid_cell_value
      Object_Name current_grid_cell_value =
          curr_wall_grid_row->world_object_names[grid_x];

      if (strcmp(current_grid_cell_value, EMPTY_GRID_CELL_VALUE) != 0) {
        is_wall_hit = 1;
        break;
      }
    }

    Scalar ray_length =
        sqrt(pow(ray.start.x - ray.end.x, 2) + pow(ray.start.y - ray.end.y, 2));
    Point_1D ray_screen_position_x =
        ((curr_angle_deg - start_angle) / PLAYER_FOV_DEG) * (WINDOW_W / 2) +
        WINDOW_W / 4;
    Scalar perpendicular_distance = ray_length * cos(theta);
    Scalar wall_vertical_strip_height =
        (GRID_CELL_SIZE * WINDOW_H) / perpendicular_distance;
    Scalar vertical_strip_width =
        (WINDOW_W / 2) / ((end_angle - start_angle) / PLAYER_FOV_DEG_INC);

    Scalar wall_vertical_offset = (WINDOW_H - wall_vertical_strip_height) / 2;

    // FLOOR
    Scalar floor_start_y = wall_vertical_offset + wall_vertical_strip_height;
    Scalar floor_vertical_strip_height = WINDOW_H - floor_start_y;

    // For each vertical pixel in the floor strip
    for (int screen_y = floor_start_y; screen_y < WINDOW_H; screen_y++) {
      // Calculate distance to the point on the floor
      Scalar distance = (WINDOW_H / 2.0f) / (screen_y - WINDOW_H / 2.0f);

      // Scale by the player's height (distance to projection plane)
      distance *= GRID_CELL_SIZE; // Adjust this factor as needed

      Point_1D floor_world_x =
          (player.rect.x + PLAYER_W / 2) + (x_dir / cos(theta)) * distance;
      Point_1D floor_world_y =
          (player.rect.y + PLAYER_H / 2) + (y_dir / cos(theta)) * distance;

      // Fix grid position calculation using floorf()
      IPoint_1D floor_grid_x = floorf(floor_world_x / GRID_CELL_SIZE);
      IPoint_1D floor_grid_y = floorf(floor_world_y / GRID_CELL_SIZE);

      // Calculate texture coordinates
      Point_1D texture_x = (int)(floor_world_x) % TEXTURE_PIXEL_W;
      Point_1D texture_y = (int)(floor_world_y) % TEXTURE_PIXEL_H;

      // Create source and destination rectangles for this pixel
      SDL_FRect src_rect = {.x = texture_x, .y = texture_y, .w = 1, .h = 1};

      SDL_FRect dst_rect = {.x = ray_screen_position_x,
                            .y = screen_y,
                            .w = vertical_strip_width,
                            .h = 1};

      Jagged_Row *curr_floor_grid_row;
      curr_floor_grid_row = &floor_grid->rows[floor_grid_y];
      for (size_t i = 0; i < world_objects_container->length; i++) {
        if (strcmp(curr_floor_grid_row->world_object_names[floor_grid_x],
                   world_objects_container->data[i]->name) == 0) {
          SDL_RenderTexture(renderer,
                            world_objects_container->data[i]->textures.data[0],
                            &src_rect, &dst_rect);
          break;
        }
      }
    }

    // /*
    //  * Conversions to screen positions
    //  */
    // Scalar ray_perp_distance =
    //     calculate_ray_perpendicular_distance(&ray, theta);

    // // SCREEN
    // Point_1D ray_screen_position_x =
    //     ((curr_angle_deg - start_angle) / PLAYER_FOV_DEG) * (WINDOW_W /
    //     2) + WINDOW_W / 4;
    // Scalar wall_vertical_strip_height =
    //     (GRID_CELL_SIZE * WINDOW_H) / ray_perp_distance;
    // Scalar vertical_strip_width =
    //     (WINDOW_W / 2) / ((end_angle - start_angle) /
    //     PLAYER_FOV_DEG_INC);

    // // Center line vertically
    // Scalar wall_vertical_offset = (WINDOW_H - wall_vertical_strip_height)
    // / 2;

    // /*
    //  * DRAW FLOORS
    //  */
    // Scalar floor_start_y = wall_vertical_offset +
    // wall_vertical_strip_height;

    // // For each vertical pixel in the floor strip
    // for (int screen_y = floor_start_y; screen_y < WINDOW_H; screen_y++) {

    //   // Calculate distance to the point on the floor
    //   Scalar distance = (WINDOW_H / 2.0f) / (screen_y - WINDOW_H / 2.0f);

    //   // Scale by the player's height (distance to projection plane)
    //   distance *= GRID_CELL_SIZE; // Adjust this factor as needed

    //   Point_1D floor_world_x =
    //       (player.rect.x + PLAYER_W / 2) + (x_dir / cos(theta)) *
    //       distance;
    //   Point_1D floor_world_y =
    //       (player.rect.y + PLAYER_H / 2) + (y_dir / cos(theta)) *
    //       distance;

    //   // Calculate texture coordinates
    //   Point_1D texture_x = (int)(floor_world_x) % TEXTURE_PIXEL_W;
    //   Point_1D texture_y = (int)(floor_world_y) % TEXTURE_PIXEL_H;

    //   // Create source and destination rectangles for this pixel
    //   SDL_FRect src_rect = {.x = texture_x, .y = texture_y, .w = 1, .h =
    //   1};

    //   SDL_FRect dst_rect = {.x = ray_screen_position_x,
    //                         .y = screen_y,
    //                         .w = vertical_strip_width,
    //                         .h = 1};

    //   curr_floor_grid_row = &floor_grid->rows[grid_y];

    //   /*
    //    * Texture case handling
    //    */

    //   for (size_t i = 0; i < world_objects_container->length; i++) {

    //     if (strcmp(curr_floor_grid_row->world_object_names[grid_x],
    //                world_objects_container->data[i]->name) == 0) {
    //       SDL_RenderTexture(renderer,
    //                         world_objects_container->data[i]->textures.data[0],
    //                         &src_rect, &dst_rect);
    //       break;
    //     }
    //   }
    // }

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
    if (surface_hit == WS_VERTICAL) {
      wall_x                     = wall_y_intersection;
      Point_1D wall_x_normalized = wall_x / GRID_CELL_SIZE;
      Point_1D wall_x_offset_normalized =
          wall_x_normalized - floorf(wall_x_normalized);
      texture_x = roundf(wall_x_offset_normalized * TEXTURE_PIXEL_W);
    } else {
      wall_x                     = wall_x_intersection;
      Point_1D wall_x_normalized = wall_x / GRID_CELL_SIZE;
      Point_1D wall_x_offset_normalized =
          wall_x_normalized - floorf(wall_x_normalized);
      texture_x = roundf(wall_x_offset_normalized * TEXTURE_PIXEL_W);
    }

    SDL_FRect src_rect = {.x = texture_x, .y = 0, .w = 1, .h = TEXTURE_PIXEL_H};

    for (size_t i = 0; i < world_objects_container->length; i++) {
      if (strcmp(curr_wall_grid_row->world_object_names[grid_x],
                 world_objects_container->data[i]->name) == 0) {
        SDL_RenderTexture(renderer,
                          world_objects_container->data[i]->textures.data[0],
                          &src_rect, &wall_rect);
        break;
      }
    }
  }
}

void draw_player(void) {
  SDL_SetRenderDrawColor(renderer, 100, 0, 255, 255);
  draw_player_rect();
  draw_player_direction();
}

static void draw_jagged_grid(void) {
  for (size_t i = 0; i < wall_grid->length; i++) {

    Jagged_Row *current_row = &wall_grid->rows[i];
    SDL_FRect   black_rects[current_row->length];
    SDL_FRect   white_rects[current_row->length];
    int         black_count = 0;
    int         white_count = 0;
    float       offset      = 0.1f;
    for (size_t j = 0; j < current_row->length; j++) {
      SDL_FRect rect;
      rect.h = GRID_CELL_SIZE * (1.0f - offset);
      rect.w = GRID_CELL_SIZE * (1.0f - offset);
      rect.x = (j * GRID_CELL_SIZE) + (GRID_CELL_SIZE * offset / 2);
      rect.y = (i * GRID_CELL_SIZE) + (GRID_CELL_SIZE * offset / 2);
      if (strcmp(wall_grid->rows[i].world_object_names[j],
                 EMPTY_GRID_CELL_VALUE) != 0) {
        black_rects[black_count++] = rect;
      } else {
        white_rects[white_count++] = rect;
      }
    }
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRects(renderer, white_rects, white_count);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRects(renderer, black_rects, black_count);
  }
}

void rotate_player(Rotation_Type rotation, float delta_time) {
  player.angle    = player.angle + (rotation * PLAYER_ROTATION_STEP *
                                 PLAYER_ROTATION_SPEED * delta_time);
  player.angle    = (player.angle < 0)     ? 360
                    : (player.angle > 360) ? 0
                                           : player.angle;
  Radians radians = convert_deg_to_rads(player.angle);
  player.delta.x  = cos(radians) * PLAYER_MOTION_DELTA_MULTIPLIER;
  player.delta.y  = sin(radians) * PLAYER_MOTION_DELTA_MULTIPLIER;
}

Grid_Hit_Box convert_world_position_to_grid_position(Point_2D *world_point,
                                                     float     offset) {
  Hit_Box player_hit_box_world = {// Top-left
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

void move_player(float direction, bool is_sprinting, float delta_time) {
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

  Grid_Hit_Box player_hit_box_grid = convert_world_position_to_grid_position(
      &new_pos, PLAYER_INTERACTION_DISTANCE);

  Jagged_Row *tl_grid_cell_row = &wall_grid->rows[player_hit_box_grid.tl.y];
  Jagged_Row *tr_grid_cell_row = &wall_grid->rows[player_hit_box_grid.tr.y];
  Jagged_Row *bl_grid_cell_row = &wall_grid->rows[player_hit_box_grid.bl.y];
  Jagged_Row *br_grid_cell_row = &wall_grid->rows[player_hit_box_grid.br.y];

  const Object_Name tl_grid_cell_value =
      tl_grid_cell_row->world_object_names[player_hit_box_grid.tl.x];
  const Object_Name tr_grid_cell_value =
      tr_grid_cell_row->world_object_names[player_hit_box_grid.tr.x];
  const Object_Name bl_grid_cell_value =
      bl_grid_cell_row->world_object_names[player_hit_box_grid.bl.x];
  const Object_Name br_grid_cell_value =
      br_grid_cell_row->world_object_names[player_hit_box_grid.br.x];

  if ((strcmp(tl_grid_cell_value, EMPTY_GRID_CELL_VALUE) == 0) &&
      (strcmp(tr_grid_cell_value, EMPTY_GRID_CELL_VALUE) == 0) &&
      (strcmp(bl_grid_cell_value, EMPTY_GRID_CELL_VALUE) == 0) &&
      (strcmp(br_grid_cell_value, EMPTY_GRID_CELL_VALUE) == 0)) {
    player.rect.x = new_pos.x;
    player.rect.y = new_pos.y;
  }
}

uint8_t get_kb_arrow_input_state(void) {
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

void handle_player_movement(float delta_time) {
  uint8_t arrows_state = get_kb_arrow_input_state();
  bool    is_sprinting = false;
  if (keyboard_state[SDL_SCANCODE_LSHIFT] ||
      keyboard_state[SDL_SCANCODE_RSHIFT]) {
    is_sprinting = true;
  }
  if (arrows_state & KEY_LEFT) {
    rotate_player(ANTI_CLOCKWISE, delta_time);
  }
  if (arrows_state & KEY_RIGHT) {
    rotate_player(CLOCKWISE, delta_time);
  }
  if (arrows_state & KEY_UP) {
    move_player(FORWARDS, is_sprinting, delta_time);
  }
  if (arrows_state & KEY_DOWN) {
    move_player(BACKWARDS, is_sprinting, delta_time);
  }
}

void update_display(void) {
  SDL_SetRenderDrawColor(renderer, 30, 0, 30, 255);
  SDL_RenderClear(renderer);
  // draw_jagged_grid();
  // draw_player();
  cast_rays_from_player();
  SDL_RenderPresent(renderer);
}

void run_game_loop(void) {
  bool     loopShouldStop = false;
  uint64_t previous_time  = SDL_GetTicks();

  while (!loopShouldStop) {
    uint64_t current_time = SDL_GetTicks();
    float    delta_time =
        (current_time - previous_time) / 1000.0f; // Convert to seconds
    previous_time = current_time;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        loopShouldStop = true;
      }
    }

    handle_player_movement(delta_time);
    update_display();
  }
}

int main() {
  const char *title = "2.5D Raycasting Game Engine";

  setup_sdl(title, WINDOW_W, WINDOW_H, SDL_WINDOW_RESIZABLE, &window,
            &renderer);
  world_objects_container =
      setup_engine_textures(renderer, "./manifests/texture_manifest.json");

  wall_grid  = read_grid_csv_file("./assets/levels/1/level-1-walls.csv");
  floor_grid = read_grid_csv_file("./assets/levels/1/level-1-floors.csv");

  print_jagged_grid(floor_grid);

  player_init();
  keyboard_state = SDL_GetKeyboardState(NULL);
  run_game_loop();

  free_jagged_grid(wall_grid);
  free_jagged_grid(floor_grid);
  cleanup_world_objects(world_objects_container);

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  SDL_Quit();

  return 0;
}