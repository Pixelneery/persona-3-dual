#pragma once
#include "controllers/MusicController.h"
#include "controllers/VideoController.h"
#include "controllers/AnimationController.h"
#include "models/character.h"
#include "components/PauseMenuComponent.h"

// variables
extern volatile int frame;
extern int fps;
extern int fpsTimer;

// controllers
extern MusicController musicCtrl;
extern VideoController videoCtrl;
extern AnimationController characterAnimationCtrl;
extern const unsigned int *bitmapsCharacter[MODEL_CHARACTER_TEX_COUNT];

// components
extern PauseMenuComponent pauseMenuCmpt;
extern bool enableBillboards;
extern bool enableDebugPrint;
extern bool enableCharacterAnim;
extern bool isPauseMenuActive;
