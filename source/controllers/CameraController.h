#pragma once

#include "core/geometry.h"
#include "core/structs.h"
#include <nds.h>

// !Todo replace floats with fixed point math for camera position and target position.
class CameraController
{
  public:
    void configure(const CameraConfig& config);

    void setMode(CameraMode mode);

    void setPath(const CameraPath* path);

    CameraPosition update(u32 keys, const CharacterPosition& charPos);

    CameraMode getMode() const
    {
        return mode;
    }
    float getAngle() const
    {
        return angle;
    }
    bool isPathComplete() const
    {
        return pathDone;
    }

    float getMovementAngle(const CharacterPosition& charPos) const;

  private:
    CameraMode mode = CameraMode::Follow;

    Vec3<float> currentPos = {};
    Vec3<float> targetPos = {};

    float angle = 0.0f;
    float distance = 1.5f;
    float height = 0.6f;
    float lookAhead = 0.5f;
    float angleIncrement = 0.05f;
    float freeCameraSpeed = 0.02f;

    // Path playback state
    const CameraPath* path = nullptr;
    int pathFrame = 0;
    int pathKeyIndex = 0;
    bool pathDone = false;

    // Free mode state
    bool freeInitialised = false;
};
