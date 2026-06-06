#include "SpriteController.h"
#include "core/globals.h"
#include "data/sprite_db.h"

bool SpriteController::switchSpriteImpl(SpriteType type, int spriteId, SpriteRegister* out)
{
    std::string filename = getSpriteFilename(type, spriteId);
    if (filename.empty())
    {
        return false;
    }

    GraphicAsset asset = graphicsCtrl.loadGrit(fatBasePath + "graphics/MenuHUD/sprites/" + filename);
    loadedAssets.push_back(asset);

    out->id = spriteId;
    out->tiles = asset.tiles;
    out->tilesLen = asset.tilesLen;
    out->pal = asset.pal;
    out->palLen = asset.palLen;

    return true;
}

void SpriteController::unloadAll()
{
    for (size_t i = 0; i < loadedAssets.size(); ++i)
    {
        graphicsCtrl.unloadGrit(loadedAssets[i]);
    }
    loadedAssets.clear();
}
