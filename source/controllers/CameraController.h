#pragma once

#include <nds.h>
#include "core/geometry.h"

enum class CameraMode
{
    Free,
    Static,
    Follow,
    Path
};

// !Todo replace floats with fixed point math for camera position and target position.
class CameraController
{
    public:

    void init(float initialAngle, const Vec3<float>& charPos);

    void setMode(CameraMode mode);

    CameraMode getMode() const
    {
        return mode;
    }

    float getAngle() const
    {
        return angle;
    }

    private:
        CameraMode mode = CameraMode::Static;
        Vec3<float> currentPos = {};
        Vec3<float> targetPos = {};
        float angle = 0.0f;
};