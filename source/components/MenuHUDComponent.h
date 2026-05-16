#pragma once
#include <nds.h>
#include "core/globals.h"
#include "core/enums.h"
#include "core/structs.h"
// menuHUD bg
#include "menuHUD.h"

class MenuHUDComponent
{
private:
    // NOTE: we can have max:
    // 1 moon
    // 1 day of the week
    // 4 numbers
    // 4 times
    // 18 skill progress items (all same sprite)

    // sprites
    Sprite sprites[28]; // enough entries for moon, day, digits, times, and repeated skill markers
    SpriteRegister moonSprite;
    SpriteRegister dayOfWeekSprite;
    SpriteRegister numberSprites[4];
    SpriteRegister timeSprites[4];
    SpriteRegister skillSprites[18];
    SpriteRegister slashSprite;

    void loadBg(int *bgId);
public:
    void loadHUD();
    void drawHUD(int *bgId);
    bool isMenuTouchArea(touchPosition *touch);
};
