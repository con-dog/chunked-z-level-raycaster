#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#define CLOCKWISE 1
#define ANTI_CLOCKWISE -1
#define BACKWARDS -1
#define FORWARDS 1

#include <math.h>

typedef int Rotation_Type;
typedef float Degrees;
typedef float Radians;

extern Radians convert_deg_to_rads(Degrees degrees);

#endif