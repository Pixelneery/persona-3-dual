#pragma once
#include <stdint.h>
#include "core/geometry.h"
#include "core/enums.h"
#include "core/structs.h"

class CharacterController
{
public:
    // 3D environment
    const int mapWidth;
    const int mapHeight;
    const uint8_t *collisionMap;

    // world
    const float tileSize;
    const float worldOffsetX;
    const float worldOffsetZ;
    const Point2D<float> characterSize;

    // movement and viewpoint
    const float speed;
    const float angleIncrement;
    const float distance;
    const float lookAhead;

    // translation (mutable)
    float height = 0.0;
    float angle = 0.0;
    Point2D<float> characterTranslate = Point2D<float>(0.0, 0.0);
    // float translateX = 0.0;
    // float translateZ = 0.0;
    float characterFacingAngle = 0.0f;

    CharacterController(
        int iMapWidth, int iMapHeight, const uint8_t *iCollisionMap,
        float iTileSize, float iWorldOffsetX, float iWorldOffsetZ, Point2D<float> iCharacterSize,
        float iSpeed, float iAngleIncrement, float iDistance, float iLookAhead,
        float iAngle, float iHeight, Point2D<float> iCharacterTranslate, float iCharacterFacingAngle) : mapWidth(iMapWidth),
                                                                                         mapHeight(iMapHeight),
                                                                                         collisionMap(iCollisionMap),
                                                                                         tileSize(iTileSize),
                                                                                         worldOffsetX(iWorldOffsetX),
                                                                                         worldOffsetZ(iWorldOffsetZ),
                                                                                         characterSize(iCharacterSize),
                                                                                         speed(iSpeed),
                                                                                         angleIncrement(iAngleIncrement),
                                                                                         distance(iDistance),
                                                                                         lookAhead(iLookAhead)
    {
        // set inital position
        angle = iAngle;
        height = iHeight;
        characterTranslate = iCharacterTranslate;
        characterFacingAngle = iCharacterFacingAngle;
    };

    cameraPosition update(u32 keys);
    characterPosition isCharacterAt();
    TileType isTileAt();

private:
    TileType isTileAt(int tileX, int TileY);
    bool isTileWalkable(float worldX, float worldZ);
};
