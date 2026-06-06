#include "GraphicsController.h"

void* GraphicsController::loadToRAM(const std::string& filepath, u32* outSize)
{
    // open file
    FILE* file = fopen(filepath.c_str(), "rb");
    if (!file)
    {
        return NULL;
    }

    // get file size
    fseek(file, 0, SEEK_END);
    u32 size = ftell(file);
    // return to beginning of file
    rewind(file);

    // allocate memory and read file into buffer
    void* buffer = malloc(size);
    if (buffer)
    {
        fread(buffer, 1, size, file);
    }

    fclose(file);

    // optionally return size to caller
    if (outSize)
    {
        *outSize = size;
    }

    return buffer;
}

void GraphicsController::unloadFromRAM(void* buffer)
{
    // free memory
    if (buffer)
    {
        free(buffer);
    }
}

GritAsset GraphicsController::loadGrit(const std::string& basePath)
{
    GritAsset asset = {NULL, 0, NULL, 0, NULL, 0};

    asset.tiles = loadToRAM(basePath + ".img.bin", &asset.tilesLen);
    asset.pal = loadToRAM(basePath + ".pal.bin", &asset.palLen);
    asset.map = loadToRAM(basePath + ".map.bin", &asset.mapLen);

    return asset;
}

void GraphicsController::unloadGrit(GritAsset& asset)
{
    unloadFromRAM(asset.tiles);
    unloadFromRAM(asset.pal);
    unloadFromRAM(asset.map);

    asset.tiles = NULL;
    asset.tilesLen = 0;
    asset.pal = NULL;
    asset.palLen = 0;
    asset.map = NULL;
    asset.mapLen = 0;
}
