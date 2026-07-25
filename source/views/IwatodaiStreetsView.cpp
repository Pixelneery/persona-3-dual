#include "IwatodaiStreetsView.h"

IwatodaiStreetsView::IwatodaiStreetsView()
{
    // Battle setup
    partyMemberProfiles.push_back(CharacterProfileDb::junpei);
    partyMemberProfiles.push_back(CharacterProfileDb::yukari);

    enemyProfiles.push_back(EnemyProfileDb::cowardlyMaya);
    enemyProfiles.push_back(EnemyProfileDb::mercilessMaya);
}

IwatodaiStreetsView::~IwatodaiStreetsView()
{
    enemyProfiles.clear();
    partyMemberProfiles.clear();
}

void IwatodaiStreetsView::startBattle()
{
    battleController->execute(CharacterProfileDb::player, partyMemberProfiles, enemyProfiles, battleStartCondition);
}

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
                                   angleIncrement,
                                   distance,
                                   lookAhead,
                                   angle,
                                   height,
                                   characterTranslate,
                                   characterFacingAngle,
                                   true);
}

void IwatodaiStreetsView::setMusic()
{
    musicCtrl->init((fatBasePath + "music/locations/iwatodaiStreets/changing_seasons.pcm").c_str(), 31.0f, 177.587f);
}

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

void IwatodaiStreetsView::onDialogueStart()
{
    // No dialogue currently
}
