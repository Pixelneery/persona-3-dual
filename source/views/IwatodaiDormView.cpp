#include "IwatodaiDormView.h"
#include "core/globals.h"
#include "math.h"
#include <malloc.h>
#include <nds.h>
#include <stdio.h>

// model
#include "models/character.h"
// dialogue
#include "dialogue/demo_dialogue.h"
// maps
#include "maps/iwatodai_dorm_floor_1.h"
#include <string>

static const unsigned int* loadEnvironmentBitmap(const std::string& path, GritAsset& asset)
{
    asset = graphicsCtrl.loadGrit(path);
    return reinterpret_cast<const unsigned int*>(asset.tiles);
}

// TODO: dont forget to clear in future
IwatodaiDormView::IwatodaiDormView()
    : battleParticipants(new std::vector<BattleParticipant*>({&merciless_Maya, &cowardly_Maya})),
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
    musicCtrl.init((fatBasePath + "music/iwatodai_dorm.pcm").c_str(), 0.0f, 920.973f);

    // setup character model
    characterAnimationCtrl.loadModel((fatBasePath + "models/character/character.bin").c_str());
    character_loadTextures(characterAnimationCtrl, bitmapsCharacter);

    // setup environment model
    GritAsset envTextures[IWATODAI_DORM_FLOOR_1_TEX_COUNT] = {};
    const unsigned int* bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_COUNT] = {
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002wall01", envTextures[0]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002wall02", envTextures[1]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002wall03", envTextures[2]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002door02", envTextures[3]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002kzr01", envTextures[4]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002kzr02", envTextures[5]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002obj01", envTextures[6]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002obj04", envTextures[7]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002obj07", envTextures[8]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002obj11", envTextures[9]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002wall04", envTextures[10]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002wall05", envTextures[11]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002wall06", envTextures[12]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002obj10", envTextures[13]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002step01", envTextures[14]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002step02", envTextures[15]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002kzr03", envTextures[16]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002kzr04", envTextures[17]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002door01", envTextures[18]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002obj03", envTextures[19]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002obj09", envTextures[20]),
        nullptr,
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002bolt01", envTextures[22]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002glow02", envTextures[23]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002floor01", envTextures[24]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002floor02", envTextures[25]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002floor03", envTextures[26]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002obj02", envTextures[27]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002obj05", envTextures[28]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002obj14", envTextures[29]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002obj15", envTextures[30]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002bolt02", envTextures[31]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002bolt03", envTextures[32]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002obj12", envTextures[33]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002obj13", envTextures[34]),
    };

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

    // setup character profiles
    characterProfiles.InitializeProfiles();
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
            }

            bgHide(bgSharedSlot);
            consoleClear();

            // trigger dialogue from interaction
            if (playerCtrl->isTileAt() == TileType::NEXT_SCENE)
            {
                musicCtrl.pause();
                return ViewState::DEBUG_VIEW;
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
        characterPosition charPos = playerCtrl->isCharacterAt();
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
