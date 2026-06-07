#include "IwatodaiDormView.h"
#include "core/globals.h"
#include "math.h"
#include <malloc.h>
#include <nds.h>
#include <stdio.h>

// model
#include "models/kotone.h"
#include "models/makoto.h"
// dialogue
#include "dialogue/demo_dialogue.h"
// maps
#include "maps/iwatodai_dorm_floor_1.h"
#include <string>

const unsigned int* loadEnvironmentBitmap(const std::string& path, GraphicAsset& asset)
{
    asset = graphicsCtrl.loadGrit(path);
    return reinterpret_cast<const unsigned int*>(asset.tiles);
}

void IwatodaiDormView::setMusic()
{
    musicCtrl.init((fatBasePath + "music/locations/iwatodaiDorm/iwatodai_dorm.pcm").c_str(), 0.0f, 920.973f);
}

// TODO: dont forget to clear in future
IwatodaiDormView::IwatodaiDormView()
    : battleParticipants(new std::vector<BattleParticipant*>({&mercilessMaya, &cowardlyMaya})),
      battleController(battleParticipants, &characterProfiles, battleStartCondition)
{
}

void IwatodaiDormView::init()
{
    videoSetMode(MODE_0_3D);
    videoSetModeSub(MODE_0_2D);

    // vram banks H & I are also in-use, and D is reserved for 3D environments
    vramSetBankA(VRAM_A_TEXTURE);
    vramSetBankB(VRAM_B_TEXTURE);
    vramSetBankC(VRAM_C_SUB_BG);
    vramSetBankD(VRAM_D_SUB_SPRITE);
    bgExtPaletteEnableSub();

    // main screen, 3D
    glInit();
    glEnable(GL_ANTIALIAS);
    glEnable(GL_TEXTURE_2D); // enable texturing

    glClearColor(0, 0, 0, 31);
    glClearPolyID(63);
    glClearDepth(0x7FFF);

    // set size of main screen
    glViewport(0, 0, 255, 191);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // zNear is how close the camera can see, zFar is the maximum draw distance
    gluPerspective(55, 256.0 / 192.0, 0.1, 40);

    glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK);
    glColor3b(255, 255, 255); // keep white so texture colors aren't tinted

    // setup sub screen
    bgSharedSlot = bgInitSub(0, BgType_Text8bpp, BgSize_T_256x256, 0, 1);
    bgMenuHUD = bgInitSub(2, BgType_Text8bpp, BgSize_T_256x256, 10, 3);
    dmaFillHalfWords(0, bgGetMapPtr(bgSharedSlot), 2048);
    dmaFillHalfWords(0, bgGetMapPtr(bgMenuHUD), 2048);

    // setup console
    consoleInit(&console, 1, BgType_Text4bpp, BgSize_T_256x256, 4, 5, false, true);
    consoleSelect(&console);

    // adjust sub screen image and console to sit correctly on each other
    bgSetPriority(console.bgId, 0);
    bgSetPriority(bgSharedSlot, 1);
    bgSetPriority(bgMenuHUD, 2);
    bgUpdate();

    // setup menuHUD
    // uses VRAM bank I for sprite extended palettes, VRAM H for bg palettes
    menuHUDCmpt.loadHUD();

    // setup player controller
    // TODO: add mapping
    playerCtrl = new CharacterController(IWATODAI_DORM_FLOOR_1_MAP_WIDTH,
                                         IWATODAI_DORM_FLOOR_1_MAP_HEIGHT,
                                         &iwatodai_dorm_floor_1_map[0][0],
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
                                         true);

    // setup music
    setMusic();

    // setup character model
    std::string modelPath = fatBasePath + "models/";
    characterAnimationCtrl.loadModel(
        (modelPath + (saveData.femcMode ? "kotone/kotone.bin" : "makoto/makoto.bin")).c_str());

    if (saveData.femcMode)
    {
        kotone_loadTextures(characterAnimationCtrl, (const unsigned int**)bitmapsCharacter);
    }
    else
    {
        makoto_loadTextures(characterAnimationCtrl, (const unsigned int**)bitmapsCharacter);
    }

    // setup environment model
    GraphicAsset envTextures[IWATODAI_DORM_FLOOR_1_TEX_COUNT] = {};
    const unsigned int* bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_COUNT] = {nullptr};

    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002WALL01] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002wall01",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002WALL01]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002WALL02] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002wall02",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002WALL02]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002WALL03] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002wall03",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002WALL03]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002DOOR02] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002door02",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002DOOR02]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002KZR01] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002kzr01",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002KZR01]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002KZR02] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002kzr02",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002KZR02]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002OBJ01] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002obj01",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002OBJ01]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002OBJ04] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002obj04",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002OBJ04]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002OBJ07] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002obj07",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002OBJ07]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002OBJ11] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002obj11",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002OBJ11]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002WALL04] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002wall04",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002WALL04]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002WALL05] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002wall05",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002WALL05]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002WALL06] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002wall06",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002WALL06]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002OBJ10] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002obj10",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002OBJ10]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002STEP01] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002step01",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002STEP01]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002STEP02] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002step02",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002STEP02]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002KZR03] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002kzr03",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002KZR03]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002KZR04] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002kzr04",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002KZR04]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002DOOR01] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002door01",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002DOOR01]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002OBJ03] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002obj03",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002OBJ03]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002OBJ09] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002obj09",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002OBJ09]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002SHADOW01] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002shadow01",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002SHADOW01]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002BOLT01] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002bolt01",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002BOLT01]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002GLOW02] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002glow02",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002GLOW02]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002FLOOR01] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002floor01",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002FLOOR01]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002FLOOR02] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002floor02",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002FLOOR02]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002FLOOR03] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002floor03",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002FLOOR03]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002OBJ02] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002obj02",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002OBJ02]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002OBJ05] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002obj05",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002OBJ05]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002OBJ14] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002obj14",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002OBJ14]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002OBJ15] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002obj15",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002OBJ15]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002BOLT02] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002bolt02",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002BOLT02]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002BOLT03] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002bolt03",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002BOLT03]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002OBJ12] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002obj12",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002OBJ12]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002OBJ13] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002obj13",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002OBJ13]);

    iwatodaiDormFloor1Env.load((fatBasePath + "environments/iwatodai_dorm_floor_1/iwatodai_dorm_floor_1.bin").c_str(),
                               bitmapsEnv);
    for (int i = 0; i < IWATODAI_DORM_FLOOR_1_TEX_COUNT; ++i)
    {
        graphicsCtrl.unloadGrit(envTextures[i]);
    }
    totalPolyCount = iwatodaiDormFloor1Env.getPolyCount();

    // setup dialogue
    demo_dialogue_bg_slot = bgSharedSlot;

    // setup pause menu
    // use the same shared background slot as the demo dialogue
    pauseMenuCmpt.init(bgSharedSlot, &isPauseMenuActive);

    // setup battle menu
    battleMenuCmpt.init(-1, &isBattleMenuActive);
    prevBattleState = false;
}

ViewState IwatodaiDormView::update()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    bgUpdate();
    oamUpdate(&oamSub);

    scanKeys();
    u32 keys = keysHeld();
    u32 pressed = keysDown();

    // resume music when battle 1. is no longer active, 2. when is previously was active
    if (!battleController.isActive() && prevBattleState)
    {
        prevBattleState = false;
        IwatodaiDormView::setMusic();
    }

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
    if (!dialogueCtrl.isActive() && !battleController.isActive() && !isPauseMenuActive)
    {
        menuHUDCmpt.drawHUD(&bgMenuHUD);
        bgShow(bgMenuHUD);
    }
    // hide menuHUD if dialogue, battle, or pauseMenu is active
    else
    {
        bgHide(bgMenuHUD);
        oamClear(&oamSub, 0, 0);
    }

    // render pauseMenu
    if (isPauseMenuActive)
    {
        ViewState menuResult = pauseMenuCmpt.update(pressed);
        if (menuResult != ViewState::KEEP_CURRENT)
        {
            musicCtrl.pause();
            return menuResult;
        }
    }
    // render world
    else
    {
        // only process world input when dialogue and battle are not active
        if (!dialogueCtrl.isActive() && !battleController.isActive())
        {
            // move character
            camPos = playerCtrl->update(keys);

            // start battle
            if (keys & KEY_Y)
            {
                battleController.execute();
                prevBattleState = true;
            }

            bgHide(bgSharedSlot);
            consoleClear();

            // trigger dialogue from interaction
            if (playerCtrl->isTileAt() == TileType::NEXT_SCENE)
            {
                musicCtrl.pause();
                return ViewState::PAULOWNIA_MALL;
            }
            else if (playerCtrl->isTileAt() == TileType::PREV_SCENE)
            {
                musicCtrl.pause();
                return ViewState::IWATODAI_STREETS;
            }
        }

        // update camera position
        gluLookAt(camPos.cameraX,
                  camPos.cameraY + 0.1f,
                  camPos.cameraZ,
                  camPos.targetX,
                  camPos.targetY,
                  camPos.targetZ,
                  camPos.upX,
                  camPos.upY,
                  camPos.upZ);

        // draw environment
        glPushMatrix();
        iwatodaiDormFloor1Env.draw();
        iwatodaiDormFloor1Env.drawBillboards(enableBillboards, // billboards face camera
                                             camPos.cameraX,
                                             camPos.cameraY,
                                             camPos.cameraZ);
        glPopMatrix(1);

        // draw character
        glPushMatrix();
        // move character
        CharacterPosition charPos = playerCtrl->isCharacterAt();
        glTranslatef(charPos.x, charPos.y, charPos.z);
        glRotatef(charPos.facingAngle, 0.0f, 1.0f, 0.0f);

        // draw character
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

    // update controllers
    battleController.update(pressed);
    dialogueCtrl.update(keys);
    characterAnimationCtrl.update();
    musicCtrl.update();

    return ViewState::KEEP_CURRENT;
}

void IwatodaiDormView::cleanup()
{
    BaseView::cleanup();

    // cleanup environment
    iwatodaiDormFloor1Env.cleanup();
    // reset textures
    glDeleteTextures(1, &characterTextureId);
    // reset shared bg slot
    dmaFillHalfWords(0, bgGetMapPtr(bgSharedSlot), 2048);

    // cleanup controllers
    delete playerCtrl;
    playerCtrl = NULL;
}
