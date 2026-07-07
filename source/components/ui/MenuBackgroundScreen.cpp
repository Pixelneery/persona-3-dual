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
}

MenuBackgroundScreen* MenuBackgroundScreen::getInstance()
{
    return instance;
}
