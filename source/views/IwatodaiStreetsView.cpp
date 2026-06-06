#include "IwatodaiStreetsView.h"
#include "core/globals.h"
#include "math.h"
#include <malloc.h>
#include <nds.h>
#include <stdio.h>

// map
#include "maps/iwatodai_streets.h"
// environment
#include "environments/iwatodai_streets.h"
#include <string>

// model
#include "models/character.h"

static const unsigned int* loadEnvironmentBitmap(const std::string& path, GritAsset& asset)
{
    asset = graphicsCtrl.loadGrit(path);
    return reinterpret_cast<const unsigned int*>(asset.tiles);
}

int streetsCharacterTextureId;
iwatodai_streets_Environment iwatodaiStreetsEnv;

void IwatodaiStreetsView::init()
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

    playerCtrl = new CharacterController(IWATODAI_STREETS_MAP_WIDTH,
                                         IWATODAI_STREETS_MAP_HEIGHT,
                                         &iwatodai_streets_map[0][0],
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
    musicCtrl.init((fatBasePath + "music/changing_seasons.pcm").c_str(), 0.0f, -1.0f);

    // setup character model
    characterAnimationCtrl.loadModel((fatBasePath + "models/character/character.bin").c_str());
    character_loadTextures(characterAnimationCtrl, bitmapsCharacter);

    // setup environment model
    GritAsset envTextures[IWATODAI_STREETS_TEX_COUNT];
    const unsigned int* bitmaps[IWATODAI_STREETS_TEX_COUNT] = {
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_streets/f007_009_07", envTextures[0]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_streets/f007_009_16", envTextures[1]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_streets/f007_009_30", envTextures[2]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_streets/f007_009_25", envTextures[3]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_streets/f007_009_08", envTextures[4]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_streets/f007_009_05", envTextures[5]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_streets/f007_009_02", envTextures[6]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_streets/f007_009_04", envTextures[7]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_streets/f007_009_28", envTextures[8]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_streets/f007_009_24", envTextures[9]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_streets/f007_009_infloor", envTextures[10]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_streets/f007_009_09", envTextures[11]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_streets/f007_009_11", envTextures[12]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_streets/f007_009_10", envTextures[13]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_streets/f007_009_13", envTextures[14]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_streets/f007_009_12", envTextures[15]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_streets/f007_009_17", envTextures[16]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_streets/f007_009_18", envTextures[17]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_streets/f007_009_32", envTextures[18]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_streets/f007_009_33", envTextures[19]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_streets/f007_009_29", envTextures[20]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_streets/f007_009_27", envTextures[21]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_streets/f007_009_22", envTextures[22]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_streets/f007_009_03", envTextures[23]),
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_streets/f007_009_wood01", envTextures[24]),
    };
    iwatodaiStreetsEnv.load((fatBasePath + "environments/iwatodai_streets/iwatodai_streets.bin").c_str(), bitmaps);
    for (int i = 0; i < IWATODAI_STREETS_TEX_COUNT; ++i)
    {
        graphicsCtrl.unloadGrit(envTextures[i]);
    }
    totalPolyCount = iwatodaiStreetsEnv.getPolyCount();

    // pause menu
    pauseMenuCmpt.init(bgSharedSlot, &isPauseMenuActive);
}

ViewState IwatodaiStreetsView::update()
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
            return ViewState::IWATODAI_DORM;
        }

        gluLookAt(camPos.cameraX,
                  camPos.cameraY + 0.3f,
                  camPos.cameraZ,
                  camPos.targetX,
                  camPos.targetY,
                  camPos.targetZ,
                  camPos.upX,
                  camPos.upY,
                  camPos.upZ);

        glPushMatrix();
        iwatodaiStreetsEnv.draw();
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

void IwatodaiStreetsView::cleanup()
{
    BaseView::cleanup();

    iwatodaiStreetsEnv.cleanup();
    glDeleteTextures(1, &streetsCharacterTextureId);
    dmaFillHalfWords(0, bgGetMapPtr(bgSharedSlot), 2048);

    delete playerCtrl;
    playerCtrl = nullptr;
}
