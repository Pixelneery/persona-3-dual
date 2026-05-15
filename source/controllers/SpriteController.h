#pragma once
#include "enums.h"
#include "structs.h"
#include <stdint.h>
#include <unordered_map>
#include <unordered_set>
#include <stdexcept>
using namespace std;

class SpriteController
{
public:
    void switchSprite(SpriteType spriteType, int spriteId, SpriteRegister *spriteRegister);

private:
    // store the sprites
    static unordered_map<SpriteType, unordered_set<SpriteRegister>> registeredSprites;

    // TODO: automatically call registerSprites, populating from some sort of DB
    void registerSprites(SpriteType spriteType, unordered_set<SpriteRegister> spriteRegisterSet);
};
