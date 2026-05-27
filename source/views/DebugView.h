#include "core/BaseView.h"
#include <nds/arm9/console.h>
// controllers
#include "controllers/DialogueController.h"
#include "controllers/CharacterController.h"
// environments
#include "environments/iwatodai_dorm.h"
// battle-related
#include "./battleActions/enemies/Enemy.h"
#include "./battleActions/enemies/Cowardly_Maya.h"
#include "./battleActions/enemies/Merciless_Maya.h"
#include "./battleActions/BattleParticipant.h"
#include "./controllers/BattleController.h" // TODO: move somewhere

class DebugView : public BaseView
{
public:
    void init() override;
    ViewState update() override;
    void cleanup() override;
    DebugView();

private:
    touchPosition touch;

    // sub screen
    int bgMenuHUD;
    PrintConsole console;

    // 3D
    int characterTextureId;
    iwatodai_dorm_Environment iwatodaiDormEnv;

    // init Character Profiles
    CharacterProfiles characterProfiles;
    // Battle participants
    Cowardly_Maya cowardly_Maya;
    Merciless_Maya merciless_Maya;
    std::vector<BattleParticipant *> *battleParticipants;

    // controllers
    BattleController battleController;
    CharacterController *playerCtrl;
    // camera pos
    cameraPosition camPos;
    // world
    const float tileSize = 0.062500f;
    const float worldOffsetX = IWATODAI_DORM_WORLD_OFFSET_X;
    const float worldOffsetZ = IWATODAI_DORM_WORLD_OFFSET_Z;
    const Point2D<float> characterSize = Point2D<float>(0.1f, 0.1f);
    // movement and viewpoint
    const float speed = 0.02f;
    const float angleIncrement = 0.05f;
    const float distance = 0.7f;
    const float lookAhead = 0.3f;
    // set character initial translation position
    const Point2D<float> characterTranslate = Point2D<float>(0, 0);
    const float height = 0.1f;
    const float angle = -1.6;
    const float characterFacingAngle = 91.67;
    DialogueController dialogueCtrl;
    int bgSharedSlot;
    int totalPolyCount = 0;
    bool isBattleMenuActive = false;
};
