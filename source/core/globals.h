#pragma once
#include "components/BattleMenuComponent.h"
#include "components/MenuHUDComponent.h"
#include "components/PauseMenuComponent.h"
#include "controllers/AnimationController.h"
#include "controllers/GraphicsController.h"
#include "controllers/MusicController.h"
#include "controllers/SaveController.h"
#include "controllers/SpriteController.h"
#include "controllers/VideoController.h"
#include "models/character.h"

class MenuHUDComponent;
class PauseMenuComponent;

// variables
extern volatile int frame;
extern int fps;
extern int fpsTimer;
extern std::string fatBasePath;
extern Save saveData;
extern unsigned int** bitmapsCharacter;

// controllers
extern SaveController saveCtrl;
extern MusicController musicCtrl;
extern VideoController videoCtrl;
extern AnimationController characterAnimationCtrl;
extern SpriteController spriteCtrl;
extern GraphicsController graphicsCtrl;

// components
extern PauseMenuComponent pauseMenuCmpt;
extern bool enableBillboards;
extern bool enableDebugPrint;
extern bool enableCharacterAnim;
extern bool isPauseMenuActive;
extern MenuHUDComponent menuHUDCmpt;
extern BattleMenuComponent battleMenuCmpt;
