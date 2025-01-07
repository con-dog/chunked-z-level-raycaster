#ifndef GRID_TYPES_H
#define GRID_TYPES_H

#include "../wall/types.h"

typedef struct
{
  int length;
  Wall_Type *elements;
} Jagged_Row;

typedef struct
{
  int num_rows;
  Jagged_Row *rows;
} Jagged_Grid;

#endif