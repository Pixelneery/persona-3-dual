#include "IwatodaiDormView.h"

// TODO: dont forget to clear in future
IwatodaiDormView::IwatodaiDormView()
{
}

void IwatodaiDormView::setMusic()
{
    musicCtrl->init((fatBasePath + "music/locations/iwatodaiDorm/iwatodai_dorm.pcm").c_str(), 1.831f, 65.907f);
}

CharacterController* IwatodaiDormView::createPlayerController()
{
    return new CharacterController(IWATODAI_DORM_FLOOR_1_MAP_WIDTH,
                                   IWATODAI_DORM_FLOOR_1_MAP_HEIGHT,
                                   &iwatodai_dorm_floor_1_map[0][0],
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

ViewState IwatodaiDormView::onTileCheck(TileType tile, u32 pressed)
{
    switch (tile)
    {
    case TileType::SCENE_1:
        return ViewState::PAULOWNIA_MALL;
    case TileType::SCENE_0:
        return ViewState::IWATODAI_STREETS;
    case TileType::C_AK:
        // start dialogue
        iprintf("\x1b[0;0HTalk");
        if (pressed & KEY_A)
        {
            prevEnvironmentState = false;
            phase = ViewPhase::Dialogue;
        }
        break;
    default:
        consoleClear();
        break;
    }
    return ViewState::KEEP_CURRENT;
}

void IwatodaiDormView::onDialogueStart()
{
    demo_yukari_kenji_argument_load();
    dialogueCtrl.setLoader(demo_yukari_kenji_argument_load_bg);
    dialogueCtrl.start(demo_yukari_kenji_argument_first());
}
