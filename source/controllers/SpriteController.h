#pragma once
#include <nds.h>
#include "core/enums.h"
#include "core/structs.h"

class SpriteController {
public:
    // iterates through the DB to find the correct sprite
    bool switchSprite(SpriteType type, int spriteId, SpriteRegister* out) const;
};
