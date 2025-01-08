#ifndef MAIN_H
#define MAIN_H

// Input helpers
#define KEY_UP (1 << 0)
#define KEY_DOWN (1 << 1)
#define KEY_LEFT (1 << 2)
#define KEY_RIGHT (1 << 3)

#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_rect.h>

#include "./assets/textures/constants.h"
#include "./assets/textures/bricks/brick-a/brick-a.h"
#include "./assets/textures/bricks/brick-b/brick-b.h"
#include "./assets/textures/bricks/brick-c/brick-c.h"
#include "./assets/textures/bricks/brick-d/brick-d.h"
#include "./assets/textures/bricks/mud-brick-a/mud-brick-a.h"
#include "./assets/textures/bricks/mud-brick-b/mud-brick-b.h"
#include "./assets/textures/bricks/mud-brick-c/mud-brick-c.h"
#include "./assets/textures/overgrown/overgrown-a/overgrown-a.h"
#include "./assets/textures/overgrown/overgrown-b/overgrown-b.h"

#include "./config/constants.h"
#include "./data/grid/constants.h"
#include "./data/grid/types.h"
#include "./data/wall/types.h"
#include "./io/level-io.h"
#include "./objects/types.h"
#include "./objects/player/constants.h"
#include "./objects/player/types.h"
#include "./types/algebraic-types.h"
#include "./utils/math-utils.h"

#endif
