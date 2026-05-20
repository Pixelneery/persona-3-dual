#include <nds.h>
#include <stdio.h>
#include "core/globals.h"
#include "core/enums.h"
#include "SignContractView.h"

// assets
#include "contract.h"
// sfx
#include "soundbank.h"

// DEBUG
bool isLastName = true;
bool isNameConfirmed = false;
std::string lastName;
std::string firstName;

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
    // TODO: add correct music
    musicCtrl.init((fatBasePath + "music/aria_of_the_soul.pcm").c_str(), 0.0f, 164.940f);

    // set video mode for 2 text layers and 2 extended rotation layer
    videoSetMode(MODE_5_2D);
    // set sub video mode for 4 text layers
    videoSetModeSub(MODE_0_2D);

    // map vram bank A to main engine background (slot 0)
    vramSetBankA(VRAM_A_MAIN_BG_0x06000000);
    vramSetBankD(VRAM_D_MAIN_BG_0x06020000);

    // enable extended palettes
    bgExtPaletteEnable();

    // initialize backgrounds
    // check https://mtheall.com/vram.html to ensure bg fit in vram
    bg[0] = bgInit(0, BgType_Text8bpp, BgSize_T_256x256, 9, 2);

    // need to set priority to properly display
    // 0 is highest, 3 is lowest
    bgSetPriority(bg[0], 0); // silhouette

    // reset background vram
    dmaFillHalfWords(0, bgGetMapPtr(bg[0]), 8192);

    // copy graphics to vram
    dmaCopy(contractTiles, bgGetGfxPtr(bg[0]), contractTilesLen);

    // copy maps to vram
    dmaCopy(contractMap, bgGetMapPtr(bg[0]), contractMapLen);

    vramSetBankE(VRAM_E_LCD); // for main engine

    // copy palettes to extended palette area
    dmaCopy(contractPal, &VRAM_E_EXT_PALETTE[0][0], contractPalLen);

    // map vram to extended palette
    vramSetBankE(VRAM_E_BG_EXT_PALETTE);

    // setup console
    // TODO: setup properly without DemoInit() fns
    consoleDemoInit();
    keyboardDemoInit();
    keyboardShow();

    iprintf("\x1b[11;6HEnter your last name");
    iprintf("\x1b[0;0H");

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
    int key = keyboardUpdate();

    // Bksp = 8, Return = 10
    if (key == 8)
    {
        cancelSFX();
        sfxCancelHandle = musicCtrl.playSFX(SFX_CANCEL, 255, 128);

        if (isLastName)
        {
            if (!lastName.empty())
            {
                lastName.pop_back();
                iprintf("%c", key);
            }
        }
        else if (!isLastName && !isNameConfirmed)
        {
            if (firstName.empty())
            {
                isLastName = true;
                consoleClear();
                iprintf("\x1b[11;6HEnter your last name");
                iprintf("\x1b[0;0H%s", lastName.c_str());
            } else
            {
                firstName.pop_back();
                iprintf("%c", key);
            }
        }
        else
        {
            isNameConfirmed = false;
            consoleClear();
            iprintf("\x1b[11;6HEnter your first name");
            iprintf("\x1b[0;0H%s", firstName.c_str());
        }
    }
    else if (key == 10)
    {
        cancelSFX();
        sfxSelectHandle = musicCtrl.playSFX(SFX_SELECT, 255, 128);

        if (isLastName)
        {
            isLastName = false;
            consoleClear();
            iprintf("\x1b[11;5HEnter your first name");
            iprintf("\x1b[0;0H");
        }
        else if (!isNameConfirmed)
        {
            isNameConfirmed = true;
            consoleClear();
            iprintf("\x1b[11;7HConfirm your name?");

            // center the text
            int posLastName = 15 - (lastName.length()/2);
            int posFirstName = 15 - (firstName.length()/2);

            iprintf("\x1b[8;%iH%s,", posLastName, lastName.c_str());
            iprintf("\x1b[9;%iH%s", posFirstName, firstName.c_str());
        }
        else
        {
            // TODO: go to next cutscene
            cancelSFX();
            musicCtrl.pause();
            return ViewState::MAIN_MENU;
        }
    }
    else if (key > 0)
    {
        cancelSFX();
        sfxMenuHandle = musicCtrl.playSFX(SFX_MENU, 255, 128);

        if (isLastName)
        {
            lastName.push_back(key);
        }
        else
        {
            firstName.push_back(key);
        }

        iprintf("%c", key);
    }

    musicCtrl.update();
    return ViewState::KEEP_CURRENT;
}

void SignContractView::cleanup()
{
    BaseView::cleanup();
}
