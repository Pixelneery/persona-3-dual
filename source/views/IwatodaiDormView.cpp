#include "IwatodaiDormView.h"

// TODO: dont forget to clear in future
IwatodaiDormView::IwatodaiDormView()
{
}

// Test path
static const CameraPath dormTestPath = {{
    {120, -0.40f, 0.60f, 2.82f, 0.4f, 0.1f, 2.80f},
    {0, -0.40f, 0.60f, 2.82f, 0.4f, 0.1f, 2.80f},
    {60, 0.40f, 0.80f, 1.80f, 0.4f, 0.1f, 2.80f},
    {120, 1.20f, 0.60f, 2.82f, 0.4f, 0.1f, 2.80f},
    {180, 0.40f, 0.40f, 3.80f, 0.4f, 0.1f, 2.80f},
    {240, -0.40f, 0.60f, 2.82f, 0.4f, 0.1f, 2.80f},
}};

void IwatodaiDormView::configureCameraController()
{
    camConfig.mode = CameraMode::Path;
    camConfig.initialAngle = angle;
    camConfig.distance = distance;
    camConfig.height = height + 0.6f;
    camConfig.lookAhead = lookAhead;
    camConfig.angleIncrement = angleIncrement;
    cameraCtrl.setPath(&dormTestPath);
}

void IwatodaiDormView::setMusic()
{
    musicCtrl->init((fatBasePath + "music/locations/iwatodaiDorm/iwatodai_dorm.pcm").c_str(), 1.300f, -1.000f);
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
                                   height,
                                   characterTranslate,
                                   characterFacingAngle);
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
