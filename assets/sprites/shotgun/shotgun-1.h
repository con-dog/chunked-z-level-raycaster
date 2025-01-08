#ifndef SHOTGUN_1_H
#define SHOTGUN_1_H

#include "../types.h"

struct shotgun_1
{
  unsigned int width;
  unsigned int height;
  unsigned int bytes_per_pixel;
  unsigned char pixel_data[128 * 88 * 4 + 1];
};

extern const struct shotgun_1 shotgun_1;

#endif