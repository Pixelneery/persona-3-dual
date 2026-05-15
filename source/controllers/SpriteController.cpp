#include "SpriteController.h"

void SpriteController::registerSprites(SpriteType spriteType, unordered_set<SpriteRegister> spriteRegisterSet)
{
    registeredSprites.insert({spriteType,
                              spriteRegisterSet});
}

void SpriteController::switchSprite(SpriteType spriteType, int spriteId, SpriteRegister *spriteRegister)
{
    try
    {
        // try to get SpriteType
        unordered_set<SpriteRegister> spriteSet = registeredSprites.at(spriteType);

        // try to get SpriteRegister
        auto it = spriteSet.find({spriteId, -1, -1, -1, -1});

        if (it != spriteSet.end())
        {
            spriteRegister->id = it->id;
            spriteRegister->tiles = it->tiles;
            spriteRegister->tilesLen = it->tilesLen;
            spriteRegister->pal = it->pal;
            spriteRegister->palLen = it->palLen;
        }
    }
    catch (const std::out_of_range &e)
    {
    };
}
