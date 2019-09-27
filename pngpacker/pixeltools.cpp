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
#include "pixeltools.h"

void copyPixelData(uint8_t* sourceImage, int sourceImageWidth, int sourceImageHeight, int sourceRegionWidth, int sourceRegionHeight, int sourceRegionX, int sourceRegionY, uint8_t* destImage, int destImageWidth, int destImageHeight, int destRegionWidth, int destRegionHeight, int destRegionX, int destRegionY)
{
  for (int sx = 0; sx < sourceRegionWidth; sx++)
  {
    for (int sy = 0; sy < sourceRegionHeight; sy++)
    {
      int sourceOffset = getPixelOffset(sourceImageWidth, sourceImageHeight, sourceRegionX + sx, sourceRegionY + sy);
      int destOffset = getPixelOffset(destImageWidth, destImageHeight, destRegionX + sx, destRegionY + sy);

      destImage[destOffset] = sourceImage[sourceOffset];
      destImage[destOffset + 1] = sourceImage[sourceOffset + 1];
      destImage[destOffset + 2] = sourceImage[sourceOffset + 2];
      destImage[destOffset + 3] = sourceImage[sourceOffset + 3];
    }
  }
}
