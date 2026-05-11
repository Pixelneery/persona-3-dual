#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <filesystem.h>
#include <maxmod9.h>

// states
#include "core/View.h"
#include "views/DisclaimerView.h"
#include "views/IntroVideoView.h"
#include "views/IntroView.h"
#include "views/MainMenuView.h"
#include "views/IwatodaiDormView.h"
#include "views/IwatodaiStreetsView.h"

// controllers
#include "controllers/MusicController.h"
#include "controllers/VideoController.h"
#include "controllers/AnimationController.h"

// components
#include "components/PauseMenuComponent.h"

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
// controllers
MusicController musicCtrl;
VideoController videoCtrl;
AnimationController characterAnimationCtrl;
const unsigned int* bitmapsCharacter[MODEL_CHARACTER_TEX_COUNT];
// components
PauseMenuComponent pauseMenuCmpt;
bool enableBillboards = true;
bool enableCharacterAnim = true;
bool enableDebugPrint = false;
bool isPauseMenuActive = false;

View* currentView = nullptr;

void SwitchView(View* newView) {
    // cleanup any existing view
    if (currentView != nullptr) {
        currentView->Cleanup();

        // free memory
        delete currentView;
    }

    // load new view
    currentView = newView;
    if (currentView != nullptr) {
        currentView->Init();
    }
}

// fn for the interrupt
void Vblank() {
	frame++;
}

int main(int argc, char *argv[]) {
	irqSet(IRQ_VBLANK, Vblank);

    // load NitroFS
    if (!nitroFSInit(NULL)) {
        consoleDemoInit();
        iprintf("NitroFS failed!\n");
    }

    // initialize maxmod (for audio)
    mm_ds_system sys;
    sys.mod_count  = 0;
    sys.samp_count = 0;
    sys.mem_bank   = 0;
    mmInit(&sys);

    // initialize maxmod (for sfx)
    mmInitDefaultMem((mm_addr)soundbank_bin);

    // setup character model
    bitmapsCharacter[MODEL_CHARACTER_TEX_CHARACTER_TEXTURE_0] = character_texture_0Bitmap;
    bitmapsCharacter[MODEL_CHARACTER_TEX_CHARACTER_TEXTURE_1] = character_texture_1Bitmap;
    bitmapsCharacter[MODEL_CHARACTER_TEX_CHARACTER_TEXTURE_2] = character_texture_2Bitmap;
    bitmapsCharacter[MODEL_CHARACTER_TEX_CHARACTER_TEXTURE_3] = character_texture_3Bitmap;
    bitmapsCharacter[MODEL_CHARACTER_TEX_CHARACTER_TEXTURE_4] = character_texture_4Bitmap;

    // start with DisclaimerView
    // seed with DS hardware timer for reliable randomness (time() can return 0 on DS)
    TIMER0_CR = TIMER_ENABLE | TIMER_DIV_1;
    srand(TIMER0_DATA);
    SwitchView(new DisclaimerView());

    // DEBUG
    // SwitchView(new IwatodaiStreetsView());

	while(pmMainLoop()) {
		swiWaitForVBlank();

        // check state of currentView
        if (currentView != nullptr) {
            ViewState nextState = currentView->Update();
            if (nextState == ViewState::INTRO) {
                SwitchView(new IntroView());
            } else if (nextState == ViewState::MAIN_MENU) {
                SwitchView(new MainMenuView());
            } else if (nextState == ViewState::IWATODAI_DORM) {
                SwitchView(new IwatodaiDormView());
            } else if (nextState == ViewState::IWATODAI_STREETS) {
                SwitchView(new IwatodaiStreetsView());
            } else if (nextState == ViewState::DISCLAIMER) {
                SwitchView(new DisclaimerView());
            } else if (nextState == ViewState::INTRO_VIDEO) {
                const char* intros[] = {"fes.vid", "base.vid", "portable.vid"};
                const char* introFile = intros[rand() % 3];
                SwitchView(new IntroVideoView(introFile));
            }
        }

		bgUpdate();
		oamUpdate(&oamMain);
	}

	return 0;
}
