#include "SpriteController.h"
#include "data/sprite_db.h"

bool SpriteController::switchSpriteImpl(SpriteType type, int spriteId, SpriteRegister* out) const {
    for (int i = 0; i < SPRITE_DB_LEN; ++i) {
        const SpriteDBEntry& e = SPRITE_DB[i];
        if (e.type == type && e.id == spriteId) {
            out->id      = e.id;
            out->tiles   = e.tiles;
            out->tilesLen = e.tilesLen;
            out->pal     = e.pal;
            out->palLen  = e.palLen;
            return true;
        }
    }
    return false;  // not found
}
