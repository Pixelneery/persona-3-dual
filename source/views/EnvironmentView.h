#pragma once

#include "views/BaseView3D.h"
#include <nds/arm9/console.h>

// controllers
#include "components/ui/DialogueScreen.h"
#include "components/ui/MenuHUDScreen.h"
#include "controllers/CharacterController.h"
#include "controllers/DialogueController.h"
#include "controllers/GraphicsController.h"
#include "controllers/UIController.h"
// core (TileType lives here - remove this include if it's already visible
// transitively through CharacterController.h in your tree)
#include "core/enums.h"
// environments / data
#include "data/environmentDb.h"
#include "environments/Environment.h"

// Base class for every "walk around a room" view (dorm, streets, mall,
// station, ...). Owns everything that is identical across rooms: sub-screen
// and console setup, UI/pause-menu/dialogue wiring, the phase state machine,
// camera math, data-driven texture loading, and drawing/cleaning up the
// room's Environment - a single concrete class now, driven entirely by an
// EnvironmentDbEntry, so there's no per-room generated class to plug in.
//
// A concrete room view (e.g. IwatodaiDormView) only needs to supply the
// handful of things that actually differ per room, via the protected hooks
// below: which EnvironmentDbEntry it is, the map data, movement tuning,
// music path, and tile/dialogue content.
class EnvironmentView : public BaseView3D
{
  public:
    void init() override;
    ViewState update() override;
    void cleanup() override;
    void setupEnvironment() override;

  protected:
    // ---- Hooks a concrete room view MUST implement ----

    // The matching entry from g_environmentDb (texture list, bin file, world
    // bounds, billboards).
    virtual const EnvironmentDbEntry* getEnvironmentDbEntry() = 0;

    // Builds this room's CharacterController. Map data/dimensions and
    // movement feel are room-specific; world offsets should come from
    // `dbEntry` (cached before this is called - see init()).
    virtual CharacterController* createPlayerController() = 0;

    // Starts this room's background music.
    virtual void setMusic() = 0;

    // Called every frame while in ViewPhase::Environment, right after the
    // player/camera update. Return anything other than
    // ViewState::KEEP_CURRENT to leave the room (e.g. a scene-transition
    // tile). To start a conversation instead, set
    // `phase = ViewPhase::Dialogue; prevEnvironmentState = false;` and
    // return ViewState::KEEP_CURRENT.
    virtual ViewState onTileCheck(TileType tile, u32 pressed) = 0;

    // Called once when entering ViewPhase::Dialogue (after the dialogue
    // screen has already been shown). Load/start whichever script belongs
    // to the tile that was interacted with.
    virtual void onDialogueStart() = 0;

    // Optional extra per-room UI/dialogue wiring, run at the end of init().
    virtual void onSetupDialogueAndUI()
    {
    }

    // ---- Shared state available to subclasses ----
    touchPosition touch;

    int bgSharedSub1;
    int bgSharedSub2;
    int bgSharedSub3;
    PrintConsole console;

    ViewPhase phase;
    bool prevPauseState;
    bool prevDialogueState;
    bool prevEnvironmentState;

    CharacterController* playerCtrl = nullptr;
    CameraPosition camPos;

    const float tileSize = 0.062500f;

    DialogueController dialogueCtrl;

    UIController* uiCtrl = UIController::getInstance();
    GraphicsController* graphicsCtrl = GraphicsController::getInstance();
    DialogueScreen* dialogueScreen = DialogueScreen::getInstance();
    MenuHUDScreen* menuHUDScreen = MenuHUDScreen::getInstance();

    // The room's 3D geometry - one concrete class for every room, driven by
    // dbEntry (see Environment.h).
    Environment env;

    // Cached at the top of init() from getEnvironmentDbEntry(), so the rest
    // of the generic code (setupEnvironment/update/cleanup) never has to pay
    // for repeated virtual calls.
    const EnvironmentDbEntry* dbEntry = nullptr;
};
