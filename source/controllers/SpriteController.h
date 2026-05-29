#pragma once
#include "core/enums.h"
#include "core/structs.h"
#include <nds.h>

class SpriteController
{
  public:
    template <typename SpriteID> bool switchSprite(SpriteType type, SpriteID spriteId, SpriteRegister* out) const
    {
        return switchSpriteImpl(type, static_cast<int>(spriteId), out);
    }

  private:
    bool switchSpriteImpl(SpriteType type, int spriteId, SpriteRegister* out) const;
};
