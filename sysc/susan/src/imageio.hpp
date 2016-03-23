#ifndef IMAGEIO_HPP
#define IMAGEIO_HPP

#include "susan.h"
#include "input_small.h"

Image getImage() {

  Image img;

  memset(&img, 0, sizeof(img));

  //input comes from input_small.h
  img.xSize = input_width;
  img.ySize = input_height;
  memcpy(img.imageBuffer, input, input_width*input_height);

  return img;
}

#endif
