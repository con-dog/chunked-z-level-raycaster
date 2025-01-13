#ifndef GRID_TYPES_H
#define GRID_TYPES_H

typedef char *Object_Name;

typedef struct Jagged_Row {
  size_t       length;
  Object_Name *world_object_names;
} Jagged_Row;

typedef struct Jagged_Grid {
  size_t      length; // row count
  Jagged_Row *rows;
} Jagged_Grid;

// TODO ! Rename and move
typedef enum Wall_Surface {
  WS_HORIZONTAL,
  WS_VERTICAL,
} Wall_Surface;

#endif