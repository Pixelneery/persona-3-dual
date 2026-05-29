#pragma once
#include <nds.h>
#include "core/globals.h"
#include "core/enums.h"
#include "core/structs.h"
#include "menuHUD.h"
#include "menuHUDFEMC.h"

class MenuHUDComponent
{
  private:
    Sprite sprites[28];
    SpriteRegister moonSprite;
    SpriteRegister dayOfWeekSprite;
    SpriteRegister numberSprites[4];
    SpriteRegister timeSprites[4];
    SpriteRegister skillSprites[18];
    SpriteRegister slashSprite;

    void loadBg(int* bgId);

  public:
    void loadHUD();
    void drawHUD(int* bgId);
    bool isMenuTouchArea(touchPosition* touch);
};
