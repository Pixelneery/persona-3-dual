#pragma once
#include "controllers/CharacterController.h"
#include "core/View3D.h"
#include "environments/iwatodai_streets.h"
#include <nds/arm9/console.h>

class IwatodaiStreetsView : public View3D
{
  public:
    void init() override;
    ViewState update() override;
    void cleanup() override;
    void setupEnvironment() override;

  private:
    ViewPhase phase;
    touchPosition touch;

    // sub screen
    int bgSharedSub1;
    int bgSharedSub2;
    int bgSharedSub3;
    PrintConsole console;

    // 3D
    iwatodai_streets_Environment iwatodaiStreetsEnv;

    CharacterController* playerCtrl;
    CameraPosition camPos;
    const float tileSize = 0.062500f;
    const float worldOffsetX = IWATODAI_STREETS_WORLD_OFFSET_X;
    const float worldOffsetZ = IWATODAI_STREETS_WORLD_OFFSET_Z;
    const Point2D<float> characterSize = Point2D<float>(0.1f, 0.1f);
    const float speed = 0.03f;
    const float angleIncrement = 0.05f;
    const float distance = 1.0f;
    const float lookAhead = 0.2f;
    // spawn in the middle of the street, facing along it (X axis)
    const Point2D<float> characterTranslate = Point2D<float>(0.60f, 0.60f);
    const float height = 0.05f;
    const float angle = 1.5708f * 2; // 180 degrees (rad)
    const float characterFacingAngle = 0.0f;

    bool prevEnvironmentState;
};
