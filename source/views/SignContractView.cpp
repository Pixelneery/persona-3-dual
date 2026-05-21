#include <nds.h>
#include <stdio.h>
#include "core/globals.h"
#include "core/enums.h"
#include "SignContractView.h"

// assets
#include "contract.h"
// sfx
#include "soundbank.h"

void SignContractView::cancelSFX()
{
    musicCtrl.stopSFX(sfxMenuHandle);
    musicCtrl.stopSFX(sfxSelectHandle);
    musicCtrl.stopSFX(sfxCancelHandle);
}

void SignContractView::init()
{
    // set both screens to black
    setBrightness(3, -16);

    // setup music
    musicCtrl.loadSFX(SFX_MENU);
    musicCtrl.loadSFX(SFX_SELECT);
    musicCtrl.loadSFX(SFX_CANCEL);
    musicCtrl.init((fatBasePath + "music/mistic.pcm").c_str(), 0.0f, -1.0f);

    videoSetMode(MODE_5_2D);
    videoSetModeSub(MODE_0_2D);

    // map vram banks to main engine background
    vramSetBankA(VRAM_A_MAIN_BG_0x06000000);
    vramSetBankD(VRAM_D_MAIN_BG_0x06020000);
    // map vram to sub screen
    vramSetBankC(VRAM_C_SUB_BG);

    // enable extended palettes
    bgExtPaletteEnable();

    // initialize backgrounds
    bg[0] = bgInit(0, BgType_Text8bpp, BgSize_T_256x256, 9, 2);
    bgSetPriority(bg[0], 0);

    // set backgrounds
    dmaFillHalfWords(0, bgGetMapPtr(bg[0]), 8192);
    dmaCopy(contractTiles, bgGetGfxPtr(bg[0]), contractTilesLen);
    dmaCopy(contractMap, bgGetMapPtr(bg[0]), contractMapLen);

    vramSetBankE(VRAM_E_LCD);
    dmaCopy(contractPal, &VRAM_E_EXT_PALETTE[0][0], contractPalLen);
    vramSetBankE(VRAM_E_BG_EXT_PALETTE);

    // setup console
    consoleInit(&animatedConsole, 0, BgType_Text4bpp, BgSize_T_256x256, 5, 3, false, true);
    consoleInit(&console, 1, BgType_Text4bpp, BgSize_T_256x256, 2, 0, false, true);
    keyboardInit(&keyboard, 2, BgType_Text4bpp, BgSize_T_256x512, 3, 1, false, true);

    bgSetPriority(animatedConsole.bgId, 0);
    bgSetPriority(console.bgId, 1);
    bgSetPriority(keyboard.background, 2);

    keyboardShow();

    consoleSelect(&animatedConsole);
    iprintf("\x1b[11;6HEnter your last name");
    iprintf("\x1b[0;0H%s", saveData.lastName.c_str());
    consoleSelect(&console);

    // setup animated text
    REG_BLDCNT_SUB = BLEND_ALPHA | BLEND_SRC_BG0 | BLEND_DST_BACKDROP;
    REG_BLDALPHA_SUB = textAlpha | ((16 - textAlpha) << 8);

    // transition both screens from black
    for (int i = -16; i < 0; i++)
    {
        setBrightness(3, i);

        // wait a few frames
        for (int duration = 0; duration <= 2; duration++)
        {
            swiWaitForVBlank();
            musicCtrl.update();
        }
    }
}

ViewState SignContractView::update()
{
    scanKeys();
    int key = keyboardUpdate();
    int pressed = keysDown();

    // Bksp (8) or "B"
    if ((key == 8) || (pressed & KEY_B))
    {
        key = 8;
        cancelSFX();
        sfxCancelHandle = musicCtrl.playSFX(SFX_CANCEL, 255, 128);

        if (isLastName)
        {
            if (!saveData.lastName.empty())
            {
                saveData.lastName.pop_back();
                iprintf("%c", key);
            }
        }
        else if (!isLastName && !isNameConfirmed)
        {
            if (saveData.firstName.empty())
            {
                isLastName = true;
                consoleSelect(&console);
                consoleClear();

                consoleSelect(&animatedConsole);
                consoleClear();
                iprintf("\x1b[11;6HEnter your last name");

                consoleSelect(&console);
                iprintf("\x1b[0;0H%s", saveData.lastName.c_str());
            }
            else
            {
                saveData.firstName.pop_back();
                iprintf("%c", key);
            }
        }
        else
        {
            isNameConfirmed = false;
            consoleSelect(&console);
            consoleClear();

            consoleSelect(&animatedConsole);
            consoleClear();
            iprintf("\x1b[11;6HEnter your first name");

            consoleSelect(&console);
            iprintf("\x1b[0;0H%s", saveData.firstName.c_str());
        }
    }
    // Return (10) or "A"
    else if ((key == 10) || (pressed & KEY_A))
    {
        key = 10;
        cancelSFX();
        sfxSelectHandle = musicCtrl.playSFX(SFX_SELECT, 255, 128);

        if (isLastName)
        {
            isLastName = false;
            consoleSelect(&console);
            consoleClear();

            consoleSelect(&animatedConsole);
            consoleClear();
            iprintf("\x1b[11;5HEnter your first name");

            consoleSelect(&console);
            iprintf("\x1b[0;0H%s", saveData.firstName.c_str());
        }
        else if (!isNameConfirmed)
        {
            isNameConfirmed = true;
            consoleSelect(&console);
            consoleClear();

            consoleSelect(&animatedConsole);
            consoleClear();
            iprintf("\x1b[11;7HConfirm your name?");

            consoleSelect(&console);

            // center the text
            int posLastName = 15 - (saveData.lastName.length() / 2);
            int posFirstName = 15 - (saveData.firstName.length() / 2);

            iprintf("\x1b[8;%iH%s,", posLastName, saveData.lastName.c_str());
            iprintf("\x1b[9;%iH%s", posFirstName, saveData.firstName.c_str());
        }
        else
        {
            cancelSFX();
            // transition both screens to black
            for (int i = 0; i <= 16; i++)
            {
                setBrightness(3, -i);

                // wait a few frames
                for (int duration = 0; duration <= 2; duration++)
                {
                    swiWaitForVBlank();
                    musicCtrl.update();
                }
            }
            musicCtrl.pause();

            return ViewState::CUTSCENE_2;
        }
    }
    // on any other keyboard entry
    else if (key > 0)
    {
        cancelSFX();
        sfxMenuHandle = musicCtrl.playSFX(SFX_MENU, 255, 128);

        if (isLastName)
        {
            saveData.lastName.push_back(key);
        }
        else
        {
            saveData.firstName.push_back(key);
        }

        iprintf("%c", key);
    }

    // animate text
    durationCounter++;
    if (durationCounter >= duration)
    {
        durationCounter = 0;
        textAlpha += textAlphaDirection;

        // start fading out
        if (textAlpha >= 16)
        {
            textAlpha = 16;
            textAlphaDirection = -1;
        }
        // start fading in
        else if (textAlpha <= 0)
        {
            textAlpha = 0;
            textAlphaDirection = 1;
        }

        REG_BLDCNT_SUB = BLEND_ALPHA | BLEND_SRC_BG0 | BLEND_DST_BACKDROP;
        REG_BLDALPHA_SUB = textAlpha | ((16 - textAlpha) << 8);
    }

    musicCtrl.update();
    return ViewState::KEEP_CURRENT;
}

void SignContractView::cleanup()
{
    BaseView::cleanup();
}
