#pragma once
#include "core/structs.h"
#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

class GraphicsController
{
  public:
    void* loadToRAM(const std::string& filepath, u32* outSize = NULL);
    void unloadFromRAM(void* buffer);

    GritAsset loadGrit(const std::string& basePath);
    void unloadGrit(GritAsset& asset);
};
