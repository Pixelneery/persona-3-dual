#pragma once
#include "controllers/MusicController.h"
#include "controllers/VideoController.h"
#include "controllers/AnimationController.h"
#include "models/character.h"
#include "controllers/SpriteController.h"
#include "components/PauseMenuComponent.h"
#include "components/MenuHUDComponent.h"

class MenuHUDComponent;
class PauseMenuComponent;

// variables
extern volatile int frame;
extern int fps;
extern int fpsTimer;
extern std::string fatBasePath;
// TODO: save introVideoPath, names into some sort of .sav file when save support is implemented
extern std::string introVideoPath;
extern std::string lastName;
extern std::string firstName;

// controllers
extern MusicController musicCtrl;
extern VideoController videoCtrl;
extern AnimationController characterAnimationCtrl;
extern const unsigned int *bitmapsCharacter[MODEL_CHARACTER_TEX_COUNT];
extern SpriteController spriteCtrl;

// components
extern PauseMenuComponent pauseMenuCmpt;
extern bool enableBillboards;
extern bool enableDebugPrint;
extern bool enableCharacterAnim;
extern bool isPauseMenuActive;
extern MenuHUDComponent menuHUDCmpt;
