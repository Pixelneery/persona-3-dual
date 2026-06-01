#include "PaulowniaMallView.h"
#include "core/globals.h"
#include "math.h"
#include <malloc.h>
#include <nds.h>
#include <stdio.h>

// model
#include "models/character.h"
// dialogue
#include "dialogue/demo_dialogue.h"
// environment textures
#include "f008_001_acyane.h"
#include "f008_001_cd.h"
#include "f008_001_chair.h"
#include "f008_001_enter01.h"
#include "f008_001_enter02.h"
#include "f008_001_fl01.h"
#include "f008_001_fl02.h"
#include "f008_001_fl03.h"
#include "f008_001_fun01.h"
#include "f008_001_fun02.h"
#include "f008_001_fun03.h"
#include "f008_001_gafl.h"
#include "f008_001_glass.h"
#include "f008_001_glow.h"
#include "f008_001_gra.h"
#include "f008_001_gra02.h"
#include "f008_001_gsfl.h"
#include "f008_001_hasira.h"
#include "f008_001_hiho.h"
#include "f008_001_jitensya.h"
#include "f008_001_jyutan.h"
#include "f008_001_kaidan.h"
#include "f008_001_kanban.h"
#include "f008_001_kanban02.h"
#include "f008_001_kanban03.h"
#include "f008_001_kar.h"
#include "f008_001_keimark.h"
#include "f008_001_komono01.h"
#include "f008_001_komono02.h"
#include "f008_001_ligfla.h"
#include "f008_001_plantA.h"
#include "f008_001_redrect.h"
#include "f008_001_shadow.h"
#include "f008_001_sky.h"
#include "f008_001_wl01.h"
#include "f008_001_wl02.h"

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
    characterAnimationCtrl.loadModel((fatBasePath + "models/character.bin").c_str());
    character_loadTextures(characterAnimationCtrl, bitmapsCharacter);

    // setup environment model
    const unsigned int* bitmapsEnv[PAULOWNIA_MALL_TEX_COUNT] = {
        f008_001_fl02Bitmap,     f008_001_fl03Bitmap,    f008_001_gsflBitmap,     f008_001_hasiraBitmap,
        f008_001_kaidanBitmap,   f008_001_skyBitmap,     f008_001_wl01Bitmap,     f008_001_wl02Bitmap,
        f008_001_komono01Bitmap, f008_001_fun03Bitmap,   f008_001_glowBitmap,     f008_001_komono02Bitmap,
        f008_001_ligflaBitmap,   f008_001_shadowBitmap,  f008_001_enter02Bitmap,  f008_001_redrectBitmap,
        f008_001_plantABitmap,   f008_001_keimarkBitmap, f008_001_jyutanBitmap,   f008_001_fl01Bitmap,
        f008_001_gra02Bitmap,    f008_001_graBitmap,     f008_001_hihoBitmap,     f008_001_kanban03Bitmap,
        f008_001_kanbanBitmap,   f008_001_fun01Bitmap,   f008_001_fun02Bitmap,    f008_001_acyaneBitmap,
        f008_001_enter01Bitmap,  f008_001_karBitmap,     f008_001_glassBitmap,    f008_001_chairBitmap,
        f008_001_gaflBitmap,     f008_001_cdBitmap,      f008_001_kanban02Bitmap, f008_001_jitensyaBitmap,
    };

    paulowniaMallEnv.load((fatBasePath + "environments/paulownia_mall.bin").c_str(), bitmapsEnv);
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
