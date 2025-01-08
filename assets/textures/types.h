#ifndef TEXTURES_TYPES_H
#define TEXTURES_TYPES_H

typedef const struct Pixel_Image_Texture_Asset
{
  unsigned int width;
  unsigned int height;
  unsigned int bytes_per_pixel;
  unsigned char pixel_data[64 * 64 * 4 + 1];
} Pixel_Image_Texture_Asset;

#endif