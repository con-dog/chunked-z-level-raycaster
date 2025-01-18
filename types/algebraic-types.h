#ifndef ALGEBRAIC_TYPES_H
#define ALGEBRAIC_TYPES_H

/*
 * Integer
 */
typedef int IPoint_1D;
typedef int IVector_1D;
typedef int IPlane_1D;
typedef int IScalar;

typedef struct IPoint_2D
{
  IPoint_1D x;
  IPoint_1D y;
} IPoint_2D;

typedef struct IVector_2D
{
  IVector_1D x;
  IVector_1D y;
} IVector_2D;

typedef struct ILine_2D
{
  IPoint_2D start;
  IPoint_2D end;
} ILine_2D;

typedef struct IRect_2D
{
  IPoint_2D origin;
  IScalar w;
  IScalar h;
} IRect_2D;

/*
 * Floats
 */

typedef float Plane_1D;
typedef float Point_1D;
typedef float Scalar;
typedef float Vector_1D;

typedef struct Point_2D
{
  Point_1D x;
  Point_1D y;
} Point_2D;

typedef struct Vector_2D
{
  Vector_1D x;
  Vector_1D y;
} Vector_2D;

typedef struct Line_2D
{
  Point_2D start;
  Point_2D end;
} Line_2D;

typedef struct Rect_2D
{
  Point_2D origin;
  Scalar w;
  Scalar h;
} Rect_2D;

typedef struct Point3D
{
  Point_1D x;
  Point_1D y;
  Point_1D z;
} Point3D;

#endif