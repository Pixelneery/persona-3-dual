#include "IwatodaiStreetsView.h"
#include "core/globals.h"

#include "maps/iwatodai_streets.h"

#include <stdio.h>

// ----------------------------
// Construction
// ----------------------------
IwatodaiStreetsView::IwatodaiStreetsView()
{
    // ----------------------------
    // Battle setup
    // ----------------------------

    mercilessMaya = new Enemy(EnemyDb::mercilessMaya);
    cowardlyMaya = new Enemy(EnemyDb::cowardlyMaya);

    player = new Player(CharacterProfileDb::player);

    yukari = new PartyMember(CharacterProfileDb::yukari);
    junpei = new PartyMember(CharacterProfileDb::junpei);

    battleParticipants = {mercilessMaya, cowardlyMaya, player, yukari, junpei};

    enemies = {mercilessMaya, cowardlyMaya};

    partyMembers = {player, yukari, junpei};
}

// ----------------------------
// Destruction
// ----------------------------
IwatodaiStreetsView::~IwatodaiStreetsView()
{
    for (BattleParticipant* participant : battleParticipants)
    {
        delete participant;
    }

    battleParticipants.clear();
    enemies.clear();
    partyMembers.clear();
}

// ----------------------------
// Battle
// ----------------------------
void IwatodaiStreetsView::startBattle()
{
    battleController->execute(player, &partyMembers, &enemies, &battleParticipants, battleStartCondition);
}

// ----------------------------
// Camera
// ----------------------------
void IwatodaiStreetsView::configureCameraController()
{
    camConfig.mode = CameraMode::Follow;
    camConfig.initialAngle = angle;
    camConfig.distance = distance;
    camConfig.height = height + 0.6f;
    camConfig.lookAhead = lookAhead;
    camConfig.angleIncrement = angleIncrement;
}

// ----------------------------
// Player controller
// ----------------------------
CharacterController* IwatodaiStreetsView::createPlayerController()
{
    return new CharacterController(IWATODAI_STREETS_MAP_WIDTH,
                                   IWATODAI_STREETS_MAP_HEIGHT,
                                   &iwatodai_streets_map[0][0],
                                   tileSize,
                                   dbEntry->worldOffsetX,
                                   dbEntry->worldOffsetZ,
                                   characterSize,
                                   speed,
                                   height,
                                   characterTranslate,
                                   characterFacingAngle);
}

// ----------------------------
// Music
// ----------------------------
void IwatodaiStreetsView::setMusic()
{
    musicCtrl->init((fatBasePath + "music/locations/iwatodaiStreets/changing_seasons.pcm").c_str(), 31.0f, 177.587f);
}

// ----------------------------
// Tile interactions
// ----------------------------
ViewState IwatodaiStreetsView::onTileCheck(TileType tile, u32 pressed)
{
    switch (tile)
    {
    case TileType::SCENE_0:
        musicCtrl->pause();
        return ViewState::IWATODAI_DORM;

    case TileType::SCENE_1:
        musicCtrl->pause();
        return ViewState::PAULOWNIA_MALL;

    case TileType::SCENE_2:
        musicCtrl->pause();
        return ViewState::STATION;

    case TileType::SHD_W:
    {
        iprintf("\x1b[0;0HBattle Zone");

        if (pressed & KEY_A)
        {
            phase = ViewPhase::Battle;
            prevEnvironmentState = false;
        }

        break;
    }

    default:
        break;
    }

    return ViewState::KEEP_CURRENT;
}

// ----------------------------
// Dialogue
// ----------------------------
void IwatodaiStreetsView::onDialogueStart()
{
    // No dialogue currently in Iwatodai Streets.
}
