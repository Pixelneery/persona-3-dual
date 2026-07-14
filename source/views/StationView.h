#pragma once

#include "views/EnvironmentView.h"
// maps
#include "maps/station.h"
// data
#include "data/environmentDb.h"

// Only what's actually specific to this room now lives here: the map/movement
// tuning, the music path, and the tile/dialogue content. Everything else
// (screen setup, UI wiring, phase handling, camera, texture loading, the
// Environment itself, draw/cleanup) lives in EnvironmentView.
//
// Note there is no more `#include "environments/station.h"` - that generated
// class (station_Environment, STATION_TEX_* enum, world-offset macros) is
// gone. EnvironmentView owns a single, generic Environment member driven by
// stationEnvironmentDbEntry below, and worldOffsetX/Z now come from dbEntry
// at runtime instead of compile-time macros. tileSize is likewise inherited
// from EnvironmentView, not redeclared here.
class StationView : public EnvironmentView
{
  public:
    StationView();

  protected:
    const EnvironmentDbEntry* getEnvironmentDbEntry() override
    {
        return g_environmentDb[3];
    }
    CharacterController* createPlayerController() override;
    void setMusic() override;
    ViewState onTileCheck(TileType tile, u32 pressed) override;
    void onDialogueStart() override;

  private:
    // movement and viewpoint tuning (this room's feel)
    const Point2D<float> characterSize = Point2D<float>(0.1f, 0.1f);
    const float speed = 0.02f;
    const float angleIncrement = 0.05f;
    const float distance = 0.7f;
    const float lookAhead = 0.3f;
    // spawn in the middle of the street, facing along it (X axis)
    const Point2D<float> characterTranslate = Point2D<float>(-0.0175f, 1.3216f);
    const float height = 0.0f;
    const float angle = 1.5708f * 2; // 180 degrees (rad)
    const float characterFacingAngle = 180.0f;
};
