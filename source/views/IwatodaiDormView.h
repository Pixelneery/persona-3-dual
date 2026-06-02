#include "core/BaseView.h"
#include <nds/arm9/console.h>
// controllers
#include "controllers/CharacterController.h"
#include "controllers/DialogueController.h"
// environments
#include "environments/iwatodai_dorm_floor_1.h"
// battle-related
#include "./battleActions/BattleParticipant.h"
#include "./battleActions/BattleStartCondition.h"
#include "./battleActions/enemies/Cowardly_Maya.h"
#include "./battleActions/enemies/Enemy.h"
#include "./battleActions/enemies/Merciless_Maya.h"
#include "./controllers/BattleController.h" // TODO: move somewhere

class IwatodaiDormView : public BaseView
{
  public:
    void init() override;
    ViewState update() override;
    void cleanup() override;
    IwatodaiDormView();

  private:
    touchPosition touch;

    // sub screen
    int bgMenuHUD;
    PrintConsole console;

    // 3D
    int characterTextureId;
    iwatodai_dorm_floor_1_Environment iwatodaiDormFloor1Env;

    // init Character Profiles
    CharacterProfiles characterProfiles;
    // Battle participants
    Cowardly_Maya cowardly_Maya;
    Merciless_Maya merciless_Maya;
    std::vector<BattleParticipant*>* battleParticipants;

    // hardcoded for now, we will have to build a battle creater for tartarus anyways
    BattleStartCondition battleStartCondition = BattleStartCondition::Even;

    // controllers
    BattleController battleController;
    CharacterController* playerCtrl;
    // camera pos
    cameraPosition camPos;
    // world
    const float tileSize = 0.062500f;
    const float worldOffsetX = IWATODAI_DORM_FLOOR_1_WORLD_OFFSET_X;
    const float worldOffsetZ = IWATODAI_DORM_FLOOR_1_WORLD_OFFSET_Z;
    const Point2D<float> characterSize = Point2D<float>(0.1f, 0.1f);
    // movement and viewpoint
    const float speed = 0.03f;
    const float angleIncrement = 0.07f;
    const float distance = 0.8f;
    const float lookAhead = 0.2f;
    // set character initial translation position
    const Point2D<float> characterTranslate = Point2D<float>(0.4f, 2.8f);
    const float height = 0.7;
    const float angle = -1.6;
    const float characterFacingAngle = 180.0f;
    DialogueController dialogueCtrl;
    int bgSharedSlot;
    int totalPolyCount = 0;
    bool isBattleMenuActive = false;
};
