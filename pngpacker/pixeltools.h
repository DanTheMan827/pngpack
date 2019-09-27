/*
  pngpacker

  Copyright (C) 2019 Daniel Radtke (DanTheMan827)

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <iostream>

#ifndef PIXELTOOLS_H
  #define PIXELTOOLS_H
  #define getPixelOffset(width, height, x, y) ((((y) * (width)) + (x)) * 4)
  #define getPixelDataSize(width, height) (((width) * (height)) * 4)
  void copyPixelData(uint8_t *sourceImage, int sourceImageWidth, int sourceImageHeight, int sourceRegionWidth, int sourceRegionHeight, int sourceRegionX, int sourceRegionY, uint8_t *destImage, int destImageWidth, int destImageHeight, int destRegionWidth, int destRegionHeight, int destRegionX, int destRegionY);
#endif
