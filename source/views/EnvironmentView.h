#pragma once

#include "views/BaseView3D.h"
#include <nds/arm9/console.h>

// controllers
#include "components/menu/BattleMenuComponent.h"
#include "components/ui/DialogueScreen.h"
#include "components/ui/MenuHUDScreen.h"
#include "controllers/BattleController.h"
#include "controllers/CameraController.h"
#include "controllers/CharacterController.h"
#include "controllers/DialogueController.h"
#include "controllers/GraphicsController.h"
#include "controllers/UIController.h"

// core
#include "core/enums.h"

// environments / data
#include "data/environmentDb.h"
#include "environment/Environment.h"

class EnvironmentView : public BaseView3D
{
  public:
    void init() override;

    ViewState update() override;

    void cleanup() override;

    void setupEnvironment() override;

  protected:
    // -------------------------------------------------
    // Room-specific hooks
    // -------------------------------------------------

    virtual const EnvironmentDbEntry* getEnvironmentDbEntry() = 0;

    virtual CharacterController* createPlayerController() = 0;

    virtual void setMusic() = 0;

    virtual ViewState onTileCheck(TileType tile, u32 pressed) = 0;

    virtual void onDialogueStart() = 0;

    virtual void onSetupDialogueAndUI()
    {
    }

    virtual void configureCameraController()
    {
    }

    // -------------------------------------------------
    // Battle
    //
    // The room owns:
    // - enemies
    // - party members
    // - player
    // - battle start condition
    //
    // The room passes this data to BattleController.
    // EnvironmentView only controls the battle phase.
    // -------------------------------------------------

    virtual void startBattle()
    {
    }

    virtual void onBattleStart()
    {
    }

    // -------------------------------------------------
    // Shared state
    // -------------------------------------------------

    touchPosition touch;

    int bgSharedSub1;
    int bgSharedSub2;
    int bgSharedSub3;

    PrintConsole console;

    ViewPhase phase;

    bool prevPauseState = false;

    bool prevDialogueState = false;

    bool prevEnvironmentState = false;

    bool prevBattleState = false;

    bool isBattleMenuActive = false;

    CharacterController* playerCtrl = nullptr;

    CameraController cameraCtrl;

    CameraPosition camPos;

    const float tileSize = 0.062500f;

    // Override fields in configureCameraController() — same struct for all modes
    CameraConfig camConfig;

    // -------------------------------------------------
    // Controllers
    // -------------------------------------------------

    DialogueController dialogueCtrl;

    UIController* uiCtrl = UIController::getInstance();

    GraphicsController* graphicsCtrl = GraphicsController::getInstance();

    DialogueScreen* dialogueScreen = DialogueScreen::getInstance();

    MenuHUDScreen* menuHUDScreen = MenuHUDScreen::getInstance();

    BattleController* battleController = BattleController::getInstance();

    BattleMenuComponent* battleMenuCmpt = BattleMenuComponent::getInstance();

    // -------------------------------------------------
    // Environment
    // -------------------------------------------------

    Environment env;

    const EnvironmentDbEntry* dbEntry = nullptr;
};
