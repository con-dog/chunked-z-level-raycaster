#ifndef PLAYER_TYPES_H
#define PLAYER_TYPES_H

#include <SDL3/SDL_render.h>

#include "../../config/constants.h"
#include "../../types/algebraic-types.h"
#include "../../utils/math-utils.h"

typedef struct Player
{
  SDL_FRect rect;
  Vector_2D delta;
  Degrees angle; // degrees
} Player;

#endif