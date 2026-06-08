#pragma once
#include "core/enums.h"
#include "core/globals.h"
#include "core/structs.h"
#include <nds.h>

class DialogueComponent
{
  private:
    // sprites
    // TODO: reduce allocated sprite/sprite registers
    Sprite sprites[50];
    SpriteRegister calendarSprite[2];
    SpriteRegister textBox[10];
    SpriteRegister nameTag[10];
    bool bgLoaded;

    void loadBg(int* bgId);

  public:
    void loadHUD();
    void drawHUD(int* bgId);
    bool isMenuTouchArea(touchPosition* touch);
};
