#pragma once

#include "views/BaseView.h"
#include <nds/arm9/console.h>
// #include "core/globals.h"
// #include <nds.h>

// controllers
#include "components/menu/BattleMenuComponent.h"
#include "components/ui/DialogueScreen.h"
#include "components/ui/MenuHUDScreen.h"
#include "controllers/BattleController.h"
#include "controllers/CharacterController.h"
#include "controllers/DialogueController.h"
#include "controllers/GraphicsController.h"
#include "controllers/UIController.h"

// core
#include "core/enums.h"

// environments / data
#include "data/environmentDb.h"
#include "environment/Environment.h"

class EnvironmentView : public BaseView
{
  public:
    /**
     * @brief One-time setup for a room
     *
     * @note Resolves a room's EnvironmentDbEntry once into
     *       dbEntry. Everything below reads from that member instead of re-deriving it or
     *       relying on a per-room generated type. If no entry can be resolved,
     *       init() logs an error and returns immediately, since nothing below
     *       this point can run without a valid entry (setupEnvironment()
     *       immediately dereferences dbEntry->name).
     */
    void init() override;

    /**
     * @brief Per-frame update for this room's view
     *
     * @note  Advances the current ViewPhase, updates Controllers,
     *        and reports whether a phase transition to a different
     *        ViewState should occur.
     *
     * @return ViewState::KEEP_CURRENT to remain on this view for another
     *         frame, or another ViewState value to signal that the caller
     *         should transition away from this view entirely.
     */
    ViewState update() override;

    /**
     * @brief Tears down everything a room's view had set up
     */
    void cleanup() override;

    /**
     * @brief Loads and uploads a room's environment geometry and textures,
     *        driven entirely by dbEntry
     *
     * @note  No per-room texture-slot code and no per-room generated class needed.
     *
     * Loads each texture slot's texture assets to build display lists and upload
     * textures to VRAM, then unloads the texture assets. Logs a message if environment
     * loading fails, since a failed load otherwise leaves environments silently
     * rendering nothing.
     */
    void setupEnvironment();

  protected:
    // -------------------------------------------------
    // Room-specific hooks
    // -------------------------------------------------
    virtual float getCameraYOffset() const
    {
        return 0.1f;
    } // default

    virtual const EnvironmentDbEntry* getEnvironmentDbEntry() = 0;

    virtual CharacterController* createPlayerController() = 0;

    virtual void setMusic() = 0;

    virtual ViewState onTileCheck(TileType tile, u32 pressed) = 0;

    virtual void onDialogueStart() = 0;

    virtual void onSetupDialogueAndUI()
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

    CameraPosition camPos;

    const float tileSize = 0.062500f;

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
