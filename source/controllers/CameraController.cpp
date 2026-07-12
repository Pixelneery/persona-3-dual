#include "CameraController.h"
#include <math.h>

void CameraController::configure(const CameraConfig& config)
{
    mode = config.mode;
    currentPos = config.eye;
    targetPos = config.target;
    angle = config.initialAngle;
    distance = config.distance;
    height = config.height;
    lookAhead = config.lookAhead;
    angleIncrement = config.angleIncrement;
}

void CameraController::setMode(CameraMode newMode)
{
    mode = newMode;
    if (newMode == CameraMode::Path)
    {
        pathFrame = 0;
        pathKeyIndex = 0;
        pathDone = false;
    }
    if (newMode == CameraMode::Free)
    {
        freeInitialised = false;
    }
}

void CameraController::setPath(const CameraPath* p)
{
    path = p;
    pathFrame = 0;
    pathKeyIndex = 0;
    pathDone = false;
}

float CameraController::getMovementAngle(const CharacterPosition& charPos) const
{
    switch (mode)
    {
    case CameraMode::CCTV:
    case CameraMode::Static:
        return atan2f(currentPos.x - charPos.x, charPos.z - currentPos.z);
    default:
        return angle;
    }
}

CameraPosition CameraController::update(u32 keys, const CharacterPosition& charPos)
{
    CameraPosition cam = {};
    cam.upY = 1.0f;

    switch (mode)
    {
    case CameraMode::Static:
    {
        cam.cameraX = currentPos.x;
        cam.cameraY = currentPos.y;
        cam.cameraZ = currentPos.z;
        cam.targetX = targetPos.x;
        cam.targetY = targetPos.y;
        cam.targetZ = targetPos.z;
        break;
    }

    case CameraMode::CCTV:
    {
        // Eye is fixed, target tracks the character
        cam.cameraX = currentPos.x;
        cam.cameraY = currentPos.y;
        cam.cameraZ = currentPos.z;
        cam.targetX = charPos.x;
        cam.targetY = charPos.y;
        cam.targetZ = charPos.z;
        break;
    }

    case CameraMode::Follow:
    {
        if (keys & KEY_L)
            angle -= angleIncrement;
        if (keys & KEY_R)
            angle += angleIncrement;

        cam.cameraX = charPos.x + sinf(angle) * distance;
        cam.cameraY = charPos.y + height;
        cam.cameraZ = charPos.z - cosf(angle) * distance;

        cam.targetX = charPos.x - sinf(angle) * lookAhead;
        cam.targetY = charPos.y + 0.1f;
        cam.targetZ = charPos.z + cosf(angle) * lookAhead;
        break;
    }

    case CameraMode::Free:
    {
        if (!freeInitialised)
        {
            currentPos.x = charPos.x;
            currentPos.y = charPos.y + height;
            currentPos.z = charPos.z;
            freeInitialised = true;
        }

        if (keys & KEY_L)
            angle -= angleIncrement;
        if (keys & KEY_R)
            angle += angleIncrement;

        const float fwdX = -sinf(angle) * freeCameraSpeed;
        const float fwdZ = cosf(angle) * freeCameraSpeed;

        if (keys & KEY_UP)
        {
            currentPos.x += fwdX;
            currentPos.z += fwdZ;
        }
        if (keys & KEY_DOWN)
        {
            currentPos.x -= fwdX;
            currentPos.z -= fwdZ;
        }
        if (keys & KEY_RIGHT)
        {
            currentPos.x -= fwdZ;
            currentPos.z += fwdX;
        }
        if (keys & KEY_LEFT)
        {
            currentPos.x += fwdZ;
            currentPos.z -= fwdX;
        }

        cam.cameraX = currentPos.x;
        cam.cameraY = currentPos.y;
        cam.cameraZ = currentPos.z;
        cam.targetX = currentPos.x - sinf(angle);
        cam.targetY = currentPos.y;
        cam.targetZ = currentPos.z + cosf(angle);
        break;
    }

    case CameraMode::Path:
    {
        if (!path || path->keyframeCount < 2)
            break;

        pathFrame++;

        while (pathKeyIndex + 2 < path->keyframeCount && pathFrame >= path->keyframes[pathKeyIndex + 1].time)
        {
            pathKeyIndex++;
        }

        const CameraKeyframe& kf0 = path->keyframes[pathKeyIndex];
        const CameraKeyframe& kf1 = path->keyframes[pathKeyIndex + 1];

        if (pathFrame >= kf1.time && pathKeyIndex + 2 >= path->keyframeCount)
        {
            pathDone = true;
            mode = CameraMode::Follow;
            cam.cameraX = kf1.cameraX;
            cam.cameraY = kf1.cameraY;
            cam.cameraZ = kf1.cameraZ;
            cam.targetX = kf1.targetX;
            cam.targetY = kf1.targetY;
            cam.targetZ = kf1.targetZ;
            break;
        }

        int span = kf1.time - kf0.time;
        float t = (span > 0) ? (float)(pathFrame - kf0.time) / (float)span : 1.0f;

        cam.cameraX = kf0.cameraX + (kf1.cameraX - kf0.cameraX) * t;
        cam.cameraY = kf0.cameraY + (kf1.cameraY - kf0.cameraY) * t;
        cam.cameraZ = kf0.cameraZ + (kf1.cameraZ - kf0.cameraZ) * t;
        cam.targetX = kf0.targetX + (kf1.targetX - kf0.targetX) * t;
        cam.targetY = kf0.targetY + (kf1.targetY - kf0.targetY) * t;
        cam.targetZ = kf0.targetZ + (kf1.targetZ - kf0.targetZ) * t;
        break;
    }

    default:
        break;
    }

    return cam;
}
