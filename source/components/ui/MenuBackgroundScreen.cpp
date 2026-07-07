#include "MenuBackgroundScreen.h"
#include "core/globals.h"

#include <string>

MenuBackgroundScreen* MenuBackgroundScreen::instance = nullptr;

void MenuBackgroundScreen::create()
{
    if (instance == nullptr)
    {
        instance = new MenuBackgroundScreen();
    }
}

void MenuBackgroundScreen::destroy()
{
    if (instance != nullptr)
    {
        delete instance;
    }

    instance = nullptr;
}

MenuBackgroundScreen* MenuBackgroundScreen::getInstance()
{
    if (instance == nullptr)
    {
        create();
    }

    return instance;
}

void MenuBackgroundScreen::load()
{
    loadedBgIndex = MENU_BACKGROUND_SCREEN_INVALID_BG_INDEX;
}

void MenuBackgroundScreen::unload()
{
    loadedBgIndex = MENU_BACKGROUND_SCREEN_INVALID_BG_INDEX;
}

void MenuBackgroundScreen::showBackground(int bgIndex)
{
    if (bgIndex == this->loadedBgIndex)
    {
        return;
    }

    std::string backgroundName;

    switch (bgIndex)
    {
    // Akihiko
    case 0:
        backgroundName = "bgAkihiko";
        break;

    // Kenji
    case 1:
        backgroundName = "bgKenji";
        break;

    // Yukari
    case 2:
        backgroundName = "bgYukari";
        break;

    // YukariClose
    case 3:
        backgroundName = "bgYukariClose";
        break;

    default:
        return;
    }

    GraphicAsset background = this->graphicsController->loadGrit(fatBasePath + "graphics/Dialogue/backgrounds/" +
                                                                 backgroundName + "/" + backgroundName);

    dmaCopy(background.tiles, bgGetGfxPtr(bgId), background.tilesLen);
    dmaCopy(background.map, bgGetMapPtr(bgId), background.mapLen);

    vramSetBankH(VRAM_H_LCD);

    dmaCopy(background.pal, &VRAM_H_EXT_PALETTE[0][0], background.palLen);

    vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);

    this->graphicsController->unloadGrit(background);

    this->loadedBgIndex = bgIndex;
}
