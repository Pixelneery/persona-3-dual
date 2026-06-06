#include "StationView.h"
#include "core/globals.h"
#include "math.h"
#include <malloc.h>
#include <nds.h>
#include <stdio.h>

// map
#include "maps/station.h"
// environment
#include "environments/station.h"
#include <string>

// model
#include "models/character.h"

static const unsigned int* loadEnvironmentBitmap(const std::string& path, GritAsset& asset)
{
    asset = graphicsCtrl.loadGrit(path);
    return reinterpret_cast<const unsigned int*>(asset.tiles);
}

int policeStationCharacterTextureId;
station_Environment stationEnv;

void StationView::init()
{
    videoSetMode(MODE_0_3D);
    videoSetModeSub(MODE_0_2D);

    vramSetBankA(VRAM_A_TEXTURE);
    vramSetBankB(VRAM_B_TEXTURE);
    vramSetBankC(VRAM_C_SUB_BG);
    vramSetBankD(VRAM_D_SUB_SPRITE);
    bgExtPaletteEnableSub();

    glInit();
    glEnable(GL_ANTIALIAS);
    glEnable(GL_TEXTURE_2D);

    glClearColor(0, 0, 0, 31);
    glClearPolyID(63);
    glClearDepth(0x7FFF);

    glViewport(0, 0, 255, 191);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(55, 256.0 / 192.0, 0.1, 40);

    glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK);
    glColor3b(255, 255, 255);

    // sub screen console
    bgSharedSlot = bgInitSub(0, BgType_Text8bpp, BgSize_T_256x256, 0, 1);
    bgMenuHUD = bgInitSub(2, BgType_Text8bpp, BgSize_T_256x256, 10, 3);
    dmaFillHalfWords(0, bgGetMapPtr(bgSharedSlot), 2048);
    dmaFillHalfWords(0, bgGetMapPtr(bgMenuHUD), 2048);

    consoleInit(&console, 1, BgType_Text4bpp, BgSize_T_256x256, 4, 5, false, true);
    consoleSelect(&console);

    bgSetPriority(console.bgId, 0);
    bgSetPriority(bgSharedSlot, 1);
    bgSetPriority(bgMenuHUD, 2);
    bgUpdate();

    // setup menuHUD
    // uses VRAM bank I for sprite extended palettes, VRAM H for bg palettes
    menuHUDCmpt.loadHUD();

    playerCtrl = new CharacterController(STATION_MAP_WIDTH,
                                         STATION_MAP_HEIGHT,
                                         &station_map[0][0],
                                         tileSize,
                                         worldOffsetX,
                                         worldOffsetZ,
                                         characterSize,
                                         speed,
                                         angleIncrement,
                                         distance,
                                         lookAhead,
                                         angle,
                                         height,
                                         characterTranslate,
                                         characterFacingAngle,
                                         false);

    // setup music
    musicCtrl.init((fatBasePath + "music/paulownia_mall.pcm").c_str(), 0.0f, -1.0f);

    // setup character model
    characterAnimationCtrl.loadModel((fatBasePath + "models/character/character.bin").c_str());
    character_loadTextures(characterAnimationCtrl, bitmapsCharacter);

    // setup environment model
    GritAsset envTextures[STATION_TEX_COUNT] = {};
    const unsigned int* bitmaps[STATION_TEX_COUNT] = {nullptr};

    bitmaps[STATION_TEX_F008_005OBJ04] = loadEnvironmentBitmap(fatBasePath + "environments/station/f008_005obj04",
                                                               envTextures[STATION_TEX_F008_005OBJ04]);
    bitmaps[STATION_TEX_F008_005OBJ02] = loadEnvironmentBitmap(fatBasePath + "environments/station/f008_005obj02",
                                                               envTextures[STATION_TEX_F008_005OBJ02]);
    bitmaps[STATION_TEX_F008_005OBJ07] = loadEnvironmentBitmap(fatBasePath + "environments/station/f008_005obj07",
                                                               envTextures[STATION_TEX_F008_005OBJ07]);
    bitmaps[STATION_TEX_F008_005OBJ06] = loadEnvironmentBitmap(fatBasePath + "environments/station/f008_005obj06",
                                                               envTextures[STATION_TEX_F008_005OBJ06]);
    bitmaps[STATION_TEX_F008_005OBJ08] = loadEnvironmentBitmap(fatBasePath + "environments/station/f008_005obj08",
                                                               envTextures[STATION_TEX_F008_005OBJ08]);
    bitmaps[STATION_TEX_LIGHTMAP_WHITE] = loadEnvironmentBitmap(fatBasePath + "environments/station/lightmap_white",
                                                                envTextures[STATION_TEX_LIGHTMAP_WHITE]);
    bitmaps[STATION_TEX_F008_005WALL01] = loadEnvironmentBitmap(fatBasePath + "environments/station/f008_005wall01",
                                                                envTextures[STATION_TEX_F008_005WALL01]);
    bitmaps[STATION_TEX_F008_005OBJ01] = loadEnvironmentBitmap(fatBasePath + "environments/station/f008_005obj01",
                                                               envTextures[STATION_TEX_F008_005OBJ01]);
    bitmaps[STATION_TEX_F008_005OBJ03] = loadEnvironmentBitmap(fatBasePath + "environments/station/f008_005obj03",
                                                               envTextures[STATION_TEX_F008_005OBJ03]);
    bitmaps[STATION_TEX_F008_005FLOOR01] = loadEnvironmentBitmap(fatBasePath + "environments/station/f008_005floor01",
                                                                 envTextures[STATION_TEX_F008_005FLOOR01]);
    bitmaps[STATION_TEX_F008_005BOLT01] = loadEnvironmentBitmap(fatBasePath + "environments/station/f008_005bolt01",
                                                                envTextures[STATION_TEX_F008_005BOLT01]);

    stationEnv.load((fatBasePath + "environments/station/station.bin").c_str(), bitmaps);
    for (int i = 0; i < STATION_TEX_COUNT; ++i)
    {
        graphicsCtrl.unloadGrit(envTextures[i]);
    }
    totalPolyCount = stationEnv.getPolyCount();

    // pause menu
    pauseMenuCmpt.init(bgSharedSlot, &isPauseMenuActive);
}

ViewState StationView::update()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    bgUpdate();
    oamUpdate(&oamSub);

    scanKeys();
    u32 keys = keysHeld();
    u32 pressed = keysDown();

    if (pressed & KEY_START)
    {
        isPauseMenuActive = !isPauseMenuActive;
    }

    // touch input
    if (pressed & KEY_TOUCH)
    {
        touchRead(&touch);
        if (menuHUDCmpt.isMenuTouchArea(&touch))
        {
            isPauseMenuActive = true;
        }
    }

    // draw menuHUD
    if (!isPauseMenuActive)
    {
        menuHUDCmpt.drawHUD(&bgMenuHUD);
        bgShow(bgMenuHUD);
    }
    // hide menuHUD if pauseMenu is active
    else
    {
        bgHide(bgMenuHUD);
        oamClear(&oamSub, 0, 0);
    }

    if (isPauseMenuActive)
    {
        ViewState menuResult = pauseMenuCmpt.update(pressed);
        if (menuResult != ViewState::KEEP_CURRENT)
        {
            musicCtrl.pause();
            return menuResult;
        }
    }
    else
    {
        consoleClear();
        bgHide(bgSharedSlot);

        camPos = playerCtrl->update(keys);

        if (playerCtrl->isTileAt() == TileType::PREV_SCENE)
        {
            musicCtrl.pause();
            return ViewState::PAULOWNIA_MALL;
        }

        gluLookAt(camPos.cameraX,
                  camPos.cameraY,
                  camPos.cameraZ,
                  camPos.targetX,
                  camPos.targetY,
                  camPos.targetZ,
                  camPos.upX,
                  camPos.upY,
                  camPos.upZ);

        glPushMatrix();
        stationEnv.draw();
        glPopMatrix(1);

        glPushMatrix();
        characterPosition charPos = playerCtrl->isCharacterAt();
        glTranslatef(charPos.x, charPos.y, charPos.z);
        glRotatef(charPos.facingAngle, 0.0f, 1.0f, 0.0f);
        characterAnimationCtrl.render();
        glPopMatrix(1);

        glFlush(0);

        // print coordinates (64x64 area from 0,0 to 64,64)
        if (enableDebugPrint)
        {
            iprintf("\x1b[19;0H\033[31mTouch x = %04X, %04X\n", touch.rawx, touch.px);
            iprintf("\x1b[20;0H\033[31mTouch y = %04X, %04X\n", touch.rawy, touch.py);
            iprintf("\x1b[21;0H\033[31mtile(x,z): %d, %d",
                    (int)((charPos.x + worldOffsetX) / tileSize),
                    (int)((charPos.z + worldOffsetZ) / tileSize));
            iprintf("\x1b[22;0H\033[31mtranslate(x,z): %d, %d", (int)(charPos.x * 100), (int)(charPos.z * 100));
            iprintf(
                "\x1b[23;0H\033[31mangle(w,c): %d, %d", (int)(charPos.angle * 100), (int)(charPos.facingAngle * 100));
        }
    }

    musicCtrl.update();
    characterAnimationCtrl.update();

    return ViewState::KEEP_CURRENT;
}

void StationView::cleanup()
{
    BaseView::cleanup();

    stationEnv.cleanup();
    glDeleteTextures(1, &policeStationCharacterTextureId);
    dmaFillHalfWords(0, bgGetMapPtr(bgSharedSlot), 2048);

    delete playerCtrl;
    playerCtrl = nullptr;
}
