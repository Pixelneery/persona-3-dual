#pragma once
#include "components/BattleMenuComponent.h"
#include "components/MenuHUDComponent.h"
#include "components/PauseMenuComponent.h"
#include "controllers/AnimationController.h"
#include "controllers/MusicController.h"
#include "controllers/SpriteController.h"
#include "controllers/VideoController.h"
#include "models/character.h"

class MenuHUDComponent;
class PauseMenuComponent;

extern volatile int frame;
extern int fps;
extern int fpsTimer;
extern std::string fatBasePath;
extern Save saveData;

extern MusicController musicCtrl;
extern VideoController videoCtrl;
extern AnimationController characterAnimationCtrl;
extern const unsigned int* bitmapsCharacter[MODEL_CHARACTER_TEX_COUNT];
extern SpriteController spriteCtrl;

extern PauseMenuComponent pauseMenuCmpt;
extern bool enableBillboards;
extern bool enableDebugPrint;
extern bool enableCharacterAnim;
extern bool isPauseMenuActive;
extern MenuHUDComponent menuHUDCmpt;
extern BattleMenuComponent battleMenuCmpt;
