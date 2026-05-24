#pragma once
#include "core/BaseView.h"
#include "controllers/CharacterController.h"
#include "environments/station.h"
#include <nds/arm9/console.h>

class StationView : public BaseView
{
public:
    void init() override;
    ViewState update() override;
    void cleanup() override;


private:
    touchPosition touch;

    int bgMenuHUD;
    int bgSharedSlot;
    PrintConsole console;

    CharacterController *playerCtrl;
    cameraPosition camPos;
    const float tileSize = 0.062500f;
    const float worldOffsetX = STATION_WORLD_OFFSET_X;
    const float worldOffsetZ = STATION_WORLD_OFFSET_Z;
    const Point2D<float> characterSize = Point2D<float>(0.1f, 0.1f);
    const float speed = 0.02f;
    const float angleIncrement = 0.05f;
    const float distance = 0.7f;
    const float lookAhead = 0.3f;
    // spawn in the middle of the street, facing along it (X axis)
    const Point2D<float> characterTranslate = Point2D<float>(-0.0175f, 1.3216f);
    const float height = 0.0f;
    const float angle = 1.5708f; // 90 degrees in radians — faces along X
    const float characterFacingAngle = 90.0f;
    int totalPolyCount = 0;
};
