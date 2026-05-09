#include <nds.h>
#include <stdio.h>
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
#include "diss_00.h"
#include "diss_01.h"
#include "diss_02.h"
#include "diss_03.h"
#include "diss_04.h"

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
volatile bool enableBillboards = true;
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
    characterAnimationCtrl.loadModel("nitro:/models/character.bin");
    bitmapsCharacter[MODEL_CHARACTER_TEX_DISS_00] = diss_00Bitmap;
    bitmapsCharacter[MODEL_CHARACTER_TEX_DISS_01] = diss_01Bitmap;
    bitmapsCharacter[MODEL_CHARACTER_TEX_DISS_02] = diss_02Bitmap;
    bitmapsCharacter[MODEL_CHARACTER_TEX_DISS_03] = diss_03Bitmap;
    bitmapsCharacter[MODEL_CHARACTER_TEX_DISS_04] = diss_04Bitmap;

    // start with DisclaimerView
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
                SwitchView(new IntroVideoView());
            }
        }

		bgUpdate();
		oamUpdate(&oamMain);
	}

	return 0;
}
