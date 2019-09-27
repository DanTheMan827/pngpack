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

#include <fstream>
#include <iostream>
#include <vector>

#include <json/json.h>
#include <lodepng.h>
#include "pixeltools.h"

#ifndef NOLICENSE
  #include "licenses.h"
#endif

#ifndef PATH_SEPARATOR
  #define PATH_SEPARATOR '/'
#endif

#ifndef COPYRIGHT_YEAR
  #define COPYRIGHT_YEAR "2019"
#endif

bool fileExists(const char *fileName)
{
    std::ifstream infile(fileName);
    return infile.good();
}

#ifndef NOLICENSE
  int showLicense()
  {
    std::cout
      << "pngpack\n\n"
      << "Copyright (C) " << COPYRIGHT_YEAR << " Daniel Radtke (DanTheMan827\n"
      << "This program is free software: you can redistribute it and/or modify\n"
      << "it under the terms of the GNU General Public License as published by\n"
      << "the Free Software Foundation, either version 3 of the License, or\n"
      << "(at your option) any later version.\n"
      << "\n"
      << "This program is distributed in the hope that it will be useful,\n"
      << "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
      << "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
      << "GNU General Public License for more details.\n"
      << "\n"
      << "You should have received a copy of the GNU General Public License\n"
      << "along with this program.  If not, see <https://www.gnu.org/licenses/>.\n"
      << "--------------------------------------------------------------------------------\n"
      << "lodepng\n\n"
      << lodepng_LICENSE
      << "\n"
      << "--------------------------------------------------------------------------------\n"
      << "jsoncpp\n\n"
      << jsoncpp_LICENSE
      << "\n"
      ;
    
    std::flush(std::cout);
    return 0;
  }
#endif

int showUsage(const char *exename)
{
  std::cout
    << "pngpack"
    #ifdef GIT_TAG
      << " v" << GIT_TAG
    #endif
    #if defined(GIT_TAG) && defined(GIT_COMMIT)
      << ","
    #endif
    #ifdef GIT_COMMIT
      << " commit " << GIT_COMMIT
    #endif
    << "\n\nUnpack:\n  " << exename << " u packed.json packed.png unpacked/"
    << "\n\nPack:\n  " << exename << " p packed.json packed.png unpacked/"
    #ifndef NOLICENSE
      << "\n\nShow license text:\n  " << exename << " l"
    #endif
    << "\n"
    ;
  
  std::flush(std::cout);
  return 1;
}

int main(int argc, char *argv[])
{
  const char *exename = strrchr(argv[0], PATH_SEPARATOR);
  if (exename) 
  {
    // skip past the last /
    ++exename;   
  } else {
    exename = argv[0];
  }
  #ifndef NOLICENSE
    if (argc == 2 && strcmp(argv[1], "l") == 0) return showLicense();
  #endif
  if (argc < 5) return showUsage(exename);
  if (argc > 1 && (strcmp(argv[1], "u") != 0 && strcmp(argv[1], "p") != 0)) return showUsage(exename);
  
  struct stat* statBuffer;
  int imageWidth = 0;
  int imageHeight = 0;

  Json::Value jsonRoot;
  std::ifstream ifs;

  if (!fileExists(argv[2]))
  {
    printf("File not found: %s\n", argv[2]);
    return 1;
  }
  ifs.open(argv[2]);

  Json::CharReaderBuilder builder;
  builder["collectComments"] = true;
  JSONCPP_STRING errs;
  if (!parseFromStream(builder, ifs, &jsonRoot, &errs))
  {
    std::cout << errs << std::endl;
    ifs.close();
    return EXIT_FAILURE;
  }
  ifs.close();

  imageWidth = jsonRoot["texture"]["width"].asInt();
  imageHeight = jsonRoot["texture"]["height"].asInt();

  unsigned error = 0;
  uint8_t* spriteSheet = 0;
  unsigned int packWidth, packHeight;

  if (strcmp(argv[1], "u") == 0)
  {
    if (!fileExists(argv[3]))
    {
      printf("File not found: %s\n", argv[3]);
      return 1;
    }
    error = lodepng_decode32_file(&spriteSheet, &packWidth, &packHeight, argv[3]);
  } else {
    packWidth = imageWidth;
    packHeight = imageHeight;
    spriteSheet = new uint8_t[getPixelDataSize(packWidth, packHeight)];
  }

  if (error) {
    printf("error %u: %s\n", error, lodepng_error_text(error));
    return error;
  }

  for (std::string const& spriteName : jsonRoot["sprites"].getMemberNames())
  {
    std::string outFilename = std::string(argv[4]) + "/" + spriteName;
    
    int spriteX = jsonRoot["sprites"][spriteName]["textureFrame"][0].asInt();
    int spriteY = jsonRoot["sprites"][spriteName]["textureFrame"][1].asInt();
    int spriteWidth = jsonRoot["sprites"][spriteName]["textureFrame"][2].asInt();
    int spriteHeight = jsonRoot["sprites"][spriteName]["textureFrame"][3].asInt();
    
    std::cout << "Processing: " << outFilename << "\n";
    std::flush(std::cout);

    if (strcmp(argv[1], "u") == 0)
    {
      uint8_t *spriteData = new uint8_t[getPixelDataSize(spriteWidth, spriteHeight)];
      copyPixelData(spriteSheet, packWidth, packHeight, spriteWidth, spriteHeight, spriteX, spriteY, spriteData, spriteWidth, spriteHeight, spriteWidth, spriteHeight, 0, 0);
      unsigned error = lodepng::encode(outFilename, spriteData, spriteWidth, spriteHeight);

      if (error) {
        printf("error %u: %s\n", error, lodepng_error_text(error));
        return error;
      }

      delete spriteData;
    } else {
      if (!fileExists(outFilename.c_str()))
      {
        printf("File not found: %s\n", outFilename);
        return 1;
      }

      unsigned char* spriteData;
      unsigned error = 0;
      unsigned int spriteFileWidth, spriteFileHeight;
      error = lodepng_decode32_file(&spriteData, &spriteFileWidth, &spriteFileHeight, outFilename.c_str());

      if (error) {
        printf("error %u: %s\n", error, lodepng_error_text(error));
        return error;
      }

      copyPixelData(spriteData, spriteFileWidth, spriteFileHeight, spriteWidth, spriteHeight, 0, 0, spriteSheet, packWidth, packHeight, spriteWidth, spriteHeight, spriteX, spriteY);
      delete spriteData;
    }

  }

  if (strcmp(argv[1],"p") == 0)
  {
    unsigned error = lodepng::encode(argv[3], spriteSheet, packWidth, packHeight);

    if (error) {
      printf("error %u: %s\n", error, lodepng_error_text(error));
      return error;
    }
  }

  return EXIT_SUCCESS;
}
