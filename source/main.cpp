#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <filesystem.h>
#include <maxmod9.h>
#include <dirent.h>
#include <fat.h>

#include "core/enums.h"

// states
#include "core/BaseView.h"
#include "views/DisclaimerView.h"
#include "views/VideoView.h"
#include "views/IntroView.h"
#include "views/MainMenuView.h"
#include "views/IwatodaiDormView.h"
#include "views/IwatodaiStreetsView.h"
#include "views/SignContractView.h"
#include "views/StationView.h"
#include "views/DebugView.h"
#include "views/PaulowniaMallView.h"

// controllers
#include "controllers/MusicController.h"
#include "controllers/VideoController.h"
#include "controllers/AnimationController.h"

// components
#include "components/PauseMenuComponent.h"
#include "components/MenuHUDComponent.h"

// sfx
#include "soundbank_bin.h"

// character model
#include "models/character.h"
#include "character_texture_0.h"
#include "character_texture_1.h"
#include "character_texture_2.h"
#include "character_texture_3.h"
#include "character_texture_4.h"

// variables
volatile int frame = 0;
int fps = 0;
int fpsTimer = 0;
std::string fatBasePath = "";
Save saveData = {
    "reload.vid",
    "",
    ""
};
// controllers
MusicController musicCtrl;
VideoController videoCtrl;
AnimationController characterAnimationCtrl;
const unsigned int *bitmapsCharacter[MODEL_CHARACTER_TEX_COUNT];
SpriteController spriteCtrl;
// components
PauseMenuComponent pauseMenuCmpt;
bool enableBillboards = true;
bool enableCharacterAnim = true;
bool enableDebugPrint = false;
bool isPauseMenuActive = false;
MenuHUDComponent menuHUDCmpt;

BaseView *currentView = nullptr;

void SwitchView(BaseView *newView)
{
    // cleanup any existing view
    if (currentView != nullptr)
    {
        currentView->cleanup();

        // free memory
        delete currentView;
    }

    // load new view
    currentView = newView;
    if (currentView != nullptr)
    {
        currentView->init();
    }
}

// fn for the interrupt
void Vblank()
{
    frame++;
}

int main(int argc, char *argv[])
{
    irqSet(IRQ_VBLANK, Vblank);

    // Initialize DLDI/FAT instead
    if (!fatInitDefault())
    {
        consoleDemoInit();
        iprintf("FAT initialization failed!\nPlease ensure the SD card is inserted.\n");
        while (1)
            swiWaitForVBlank();
    }

    // dynamically resolve runtime path using argv[0]
    if (argc > 0 && argv[0] != nullptr)
    {
        std::string execPath(argv[0]);
        size_t lastSlash = execPath.find_last_of('/');

        if (lastSlash != std::string::npos)
        {
            fatBasePath = execPath.substr(0, lastSlash + 1) + "data/";
        }
    }

    // initialize maxmod (for audio)
    mm_ds_system sys;
    sys.mod_count = 0;
    sys.samp_count = 0;
    sys.mem_bank = 0;
    mmInit(&sys);

    // initialize maxmod (for sfx)
    mmInitDefaultMem((mm_addr)soundbank_bin);

    // setup character model
    bitmapsCharacter[MODEL_CHARACTER_TEX_CHARACTER_TEXTURE_0] = character_texture_0Bitmap;
    bitmapsCharacter[MODEL_CHARACTER_TEX_CHARACTER_TEXTURE_1] = character_texture_1Bitmap;
    bitmapsCharacter[MODEL_CHARACTER_TEX_CHARACTER_TEXTURE_2] = character_texture_2Bitmap;
    bitmapsCharacter[MODEL_CHARACTER_TEX_CHARACTER_TEXTURE_3] = character_texture_3Bitmap;
    bitmapsCharacter[MODEL_CHARACTER_TEX_CHARACTER_TEXTURE_4] = character_texture_4Bitmap;

    // use DS hardware timer for reliable randomness (time() can return 0 on DS)
    TIMER0_CR = TIMER_ENABLE | TIMER_DIV_1;
    srand(TIMER0_DATA);

    // start with DisclaimerView
    SwitchView(new DisclaimerView());

    while (pmMainLoop())
    {
        swiWaitForVBlank();

        // check state of currentView
        if (currentView != nullptr)
        {
            ViewState nextState = currentView->update();
            if (nextState == ViewState::INTRO)
            {
                SwitchView(new IntroView());
            }
            else if (nextState == ViewState::MAIN_MENU)
            {
                SwitchView(new MainMenuView());
            }
            else if (nextState == ViewState::IWATODAI_DORM)
            {
                SwitchView(new IwatodaiDormView());
            }
            else if (nextState == ViewState::IWATODAI_STREETS)
            {
                SwitchView(new IwatodaiStreetsView());
            }
            else if (nextState == ViewState::DISCLAIMER)
            {
                SwitchView(new DisclaimerView());
            }
            else if (nextState == ViewState::INTRO_VIDEO)
            {
                SwitchView(new VideoView(saveData.introVideoPath.c_str(), ViewState::INTRO));
            }
            else if (nextState == ViewState::CUTSCENE_1)
            {
                SwitchView(new VideoView("cutscene-1.vid", ViewState::SIGN_CONTRACT));
            }
            else if (nextState == ViewState::SIGN_CONTRACT)
            {
                SwitchView(new SignContractView());
            }
            else if (nextState == ViewState::CUTSCENE_2)
            {
                SwitchView(new VideoView("cutscene-2.vid", ViewState::IWATODAI_DORM));
            }
            else if (nextState == ViewState::STATION)
            {
                SwitchView(new StationView());
            }
            else if (nextState == ViewState::DEBUG_VIEW)
            {
                SwitchView(new DebugView());
            }
            else if (nextState == ViewState::PAULOWNIA_MALL)
            {
                SwitchView(new PaulowniaMallView());
            }
        }

        bgUpdate();
        oamUpdate(&oamMain);
    }

    return 0;
}
