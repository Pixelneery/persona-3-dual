#pragma once

#include "views/EnvironmentView.h"
// maps
#include "maps/iwatodai_dorm_floor_1.h"
// data
#include "data/environmentDb.h"

// Only what's actually specific to this room now lives here: the map/movement
// tuning, the music path, and the tile/dialogue content. Everything else
// (screen setup, UI wiring, phase handling, camera, texture loading, the
// Environment itself, draw/cleanup) lives in EnvironmentView.
//
// Note there is no more `#include "environments/iwatodai_dorm_floor_1.h"` -
// that generated class is gone. EnvironmentView owns a single, generic
// Environment member driven by iwatodai_dorm_floor_1EnvironmentDbEntry below.
class IwatodaiDormView : public EnvironmentView
{
  public:
    // TODO: dont forget to clear in future
    IwatodaiDormView();

  protected:
    const EnvironmentDbEntry* getEnvironmentDbEntry() override
    {
        return g_environmentDb[0];
    }
    CharacterController* createPlayerController() override;
    void setMusic() override;
    ViewState onTileCheck(TileType tile, u32 pressed) override;
    void onDialogueStart() override;

  private:
    // movement and viewpoint tuning (this room's feel)
    const Point2D<float> characterSize = Point2D<float>(0.1f, 0.1f);
    const float speed = 0.03f;
    const float angleIncrement = 0.07f;
    const float distance = 0.8f;
    const float lookAhead = 0.2f;
    // set character initial translation position
    const Point2D<float> characterTranslate = Point2D<float>(0.4f, 2.8f);
    const float height = 0.0f;
    const float angle = -1.6f;
    const float characterFacingAngle = 180.0f;
};
