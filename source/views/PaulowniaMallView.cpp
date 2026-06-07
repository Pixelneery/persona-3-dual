#include "PaulowniaMallView.h"
#include "core/globals.h"
#include "math.h"
#include <malloc.h>
#include <nds.h>
#include <stdio.h>

// model
#include "models/character.h"
#include "models/makoto.h"
// dialogue
#include "dialogue/demo_dialogue.h"
#include <string>

static const unsigned int* loadEnvironmentBitmap(const std::string& path, GraphicAsset& asset)
{
    asset = graphicsCtrl.loadGrit(path);
    return reinterpret_cast<const unsigned int*>(asset.tiles);
}

// TODO: dont forget to clear in future
PaulowniaMallView::PaulowniaMallView()
    : battleParticipants(new std::vector<BattleParticipant*>({&merciless_Maya, &cowardly_Maya})),
      battleController(battleParticipants, &characterProfiles, battleStartCondition)
{
}

void PaulowniaMallView::init()
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
    playerCtrl = new CharacterController(0,
                                         0,
                                         nullptr,
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
    musicCtrl.init((fatBasePath + "music/color_your_night.pcm").c_str(), 0.0f, 920.973f);

    // setup character model
    std::string modelPath = fatBasePath + "models/";
    characterAnimationCtrl.loadModel(
        (modelPath + (saveData.femcMode ? "character/character.bin" : "makoto/makoto.bin")).c_str());

    if (saveData.femcMode)
    {
        character_loadTextures(characterAnimationCtrl, (const unsigned int**)bitmapsCharacter);
    }
    else
    {
        makoto_loadTextures(characterAnimationCtrl, (const unsigned int**)bitmapsCharacter);
    }

    // setup environment model
    GraphicAsset envTextures[PAULOWNIA_MALL_TEX_COUNT] = {};
    const unsigned int* bitmapsEnv[PAULOWNIA_MALL_TEX_COUNT] = {nullptr};

    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_FL02] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_fl02", envTextures[PAULOWNIA_MALL_TEX_F008_001_FL02]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_FL03] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_fl03", envTextures[PAULOWNIA_MALL_TEX_F008_001_FL03]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_GSFL] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_gsfl", envTextures[PAULOWNIA_MALL_TEX_F008_001_GSFL]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_HASIRA] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_hasira", envTextures[PAULOWNIA_MALL_TEX_F008_001_HASIRA]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_KAIDAN] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_kaidan", envTextures[PAULOWNIA_MALL_TEX_F008_001_KAIDAN]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_SKY] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_sky", envTextures[PAULOWNIA_MALL_TEX_F008_001_SKY]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_WL01] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_wl01", envTextures[PAULOWNIA_MALL_TEX_F008_001_WL01]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_WL02] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_wl02", envTextures[PAULOWNIA_MALL_TEX_F008_001_WL02]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_KOMONO01] =
        loadEnvironmentBitmap(fatBasePath + "environments/paulownia_mall/f008_001_komono01",
                              envTextures[PAULOWNIA_MALL_TEX_F008_001_KOMONO01]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_FUN03] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_fun03", envTextures[PAULOWNIA_MALL_TEX_F008_001_FUN03]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_GLOW] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_glow", envTextures[PAULOWNIA_MALL_TEX_F008_001_GLOW]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_KOMONO02] =
        loadEnvironmentBitmap(fatBasePath + "environments/paulownia_mall/f008_001_komono02",
                              envTextures[PAULOWNIA_MALL_TEX_F008_001_KOMONO02]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_LIGFLA] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_ligfla", envTextures[PAULOWNIA_MALL_TEX_F008_001_LIGFLA]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_SHADOW] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_shadow", envTextures[PAULOWNIA_MALL_TEX_F008_001_SHADOW]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_ENTER02] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_enter02", envTextures[PAULOWNIA_MALL_TEX_F008_001_ENTER02]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_REDRECT] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_redrect", envTextures[PAULOWNIA_MALL_TEX_F008_001_REDRECT]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_PLANTA] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_plantA", envTextures[PAULOWNIA_MALL_TEX_F008_001_PLANTA]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_KEIMARK] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_keimark", envTextures[PAULOWNIA_MALL_TEX_F008_001_KEIMARK]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_JYUTAN] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_jyutan", envTextures[PAULOWNIA_MALL_TEX_F008_001_JYUTAN]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_FL01] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_fl01", envTextures[PAULOWNIA_MALL_TEX_F008_001_FL01]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_GRA02] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_gra02", envTextures[PAULOWNIA_MALL_TEX_F008_001_GRA02]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_GRA] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_gra", envTextures[PAULOWNIA_MALL_TEX_F008_001_GRA]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_HIHO] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_hiho", envTextures[PAULOWNIA_MALL_TEX_F008_001_HIHO]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_KANBAN03] =
        loadEnvironmentBitmap(fatBasePath + "environments/paulownia_mall/f008_001_kanban03",
                              envTextures[PAULOWNIA_MALL_TEX_F008_001_KANBAN03]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_KANBAN] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_kanban", envTextures[PAULOWNIA_MALL_TEX_F008_001_KANBAN]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_FUN01] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_fun01", envTextures[PAULOWNIA_MALL_TEX_F008_001_FUN01]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_FUN02] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_fun02", envTextures[PAULOWNIA_MALL_TEX_F008_001_FUN02]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_ACYANE] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_acyane", envTextures[PAULOWNIA_MALL_TEX_F008_001_ACYANE]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_ENTER01] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_enter01", envTextures[PAULOWNIA_MALL_TEX_F008_001_ENTER01]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_KAR] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_kar", envTextures[PAULOWNIA_MALL_TEX_F008_001_KAR]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_GLASS] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_glass", envTextures[PAULOWNIA_MALL_TEX_F008_001_GLASS]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_CHAIR] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_chair", envTextures[PAULOWNIA_MALL_TEX_F008_001_CHAIR]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_GAFL] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_gafl", envTextures[PAULOWNIA_MALL_TEX_F008_001_GAFL]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_CD] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_cd", envTextures[PAULOWNIA_MALL_TEX_F008_001_CD]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_KANBAN02] =
        loadEnvironmentBitmap(fatBasePath + "environments/paulownia_mall/f008_001_kanban02",
                              envTextures[PAULOWNIA_MALL_TEX_F008_001_KANBAN02]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_JITENSYA] =
        loadEnvironmentBitmap(fatBasePath + "environments/paulownia_mall/f008_001_jitensya",
                              envTextures[PAULOWNIA_MALL_TEX_F008_001_JITENSYA]);

    paulowniaMallEnv.load((fatBasePath + "environments/paulownia_mall/paulownia_mall.bin").c_str(), bitmapsEnv);
    for (int i = 0; i < PAULOWNIA_MALL_TEX_COUNT; ++i)
    {
        graphicsCtrl.unloadGrit(envTextures[i]);
    }
    totalPolyCount = paulowniaMallEnv.getPolyCount();

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

ViewState PaulowniaMallView::update()
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
                return ViewState::IWATODAI_STREETS;
            }
            else if (playerCtrl->isTileAt() == TileType::CHARACTER_Akihiko)
            {
                iprintf("\x1b[0;0HPress A to talk");
                if (pressed & KEY_A)
                {
                    demo_yukari_kenji_argument_load();
                    dialogueCtrl.setLoader(demo_yukari_kenji_argument_load_bg);
                    dialogueCtrl.start(demo_yukari_kenji_argument_first());
                }
            }
        }

        // update camera position
        gluLookAt(camPos.cameraX,
                  camPos.cameraY + 0.3f,
                  camPos.cameraZ,
                  camPos.targetX,
                  camPos.targetY,
                  camPos.targetZ,
                  camPos.upX,
                  camPos.upY,
                  camPos.upZ);

        // draw environment
        glPushMatrix();
        paulowniaMallEnv.draw();
        paulowniaMallEnv.drawBillboards(enableBillboards, // billboards face camera
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

void PaulowniaMallView::cleanup()
{
    BaseView::cleanup();

    // cleanup environment
    paulowniaMallEnv.cleanup();
    // reset textures
    glDeleteTextures(1, &characterTextureId);
    // reset shared bg slot
    dmaFillHalfWords(0, bgGetMapPtr(bgSharedSlot), 2048);

    // cleanup controllers
    delete playerCtrl;
    playerCtrl = NULL;
}
