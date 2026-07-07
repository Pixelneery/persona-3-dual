#include "MenuBackgroundScreen.h"

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
