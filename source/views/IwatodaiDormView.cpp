#include <nds.h>
#include <stdio.h>
#include <malloc.h>
#include "core/globals.h"
#include "math.h"
#include "IwatodaiDormView.h"

// model
#include "models/character.h"
// environment
#include "environments/iwatodai_dorm.h"
#include "texture.h"
// maps
#include "maps/iwatodai_dorm.h"
// dialogue
#include "dialogue/demo_dialogue.h"

// bgSubScreen
#include "menuHUD.h"

// TODO: move to header
int characterTextureId;
iwatodai_dorm_Environment iwatodaiDormEnv;

// TODO: dont forget to clear in future
IwatodaiDormView::IwatodaiDormView() : enemies(new std::vector<Enemy *>({&merciless_Maya, &cowardly_Maya})),
                                       battleController(&player, enemies) {}

void IwatodaiDormView::Init()
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
    bgSubScreen = bgInitSub(2, BgType_Text8bpp, BgSize_T_256x256, 10, 3);
    dmaFillHalfWords(0, bgGetMapPtr(bgSharedSlot), 2048);
    dmaFillHalfWords(0, bgGetMapPtr(bgSubScreen), 2048);

    // setup console
    consoleInit(&console, 1, BgType_Text4bpp, BgSize_T_256x256, 4, 5, false, true);
    consoleSelect(&console);

    // adjust sub screen image and console to sit correctly on each other
    bgSetPriority(console.bgId, 0);
    bgSetPriority(bgSharedSlot, 1);
    bgSetPriority(bgSubScreen, 2);

    // set bgSubScreen img
    dmaCopy(menuHUDTiles, bgGetGfxPtr(bgSubScreen), menuHUDTilesLen);
    dmaCopy(menuHUDMap, bgGetMapPtr(bgSubScreen), menuHUDMapLen);
    vramSetBankH(VRAM_H_LCD);
    dmaCopy(menuHUDPal, &VRAM_H_EXT_PALETTE[2][0], menuHUDPalLen);
    vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);
    bgShow(bgSubScreen);

    bgUpdate();

    // setup sprites
    // moon
	sprites[0] = {0, SpriteSize_32x32, SpriteColorFormat_256Color, 0, 0, 202, 20};
    // day of the week
    sprites[1] = {0, SpriteSize_32x32, SpriteColorFormat_256Color, 0, 1, 82, 143};
    // numbers
    sprites[2] = {0, SpriteSize_32x32, SpriteColorFormat_256Color, 0, 2, 0, 0};     // month (far)
    sprites[3] = {0, SpriteSize_32x32, SpriteColorFormat_256Color, 0, 2, -11, 140}; // month (close)
    sprites[4] = {0, SpriteSize_32x32, SpriteColorFormat_256Color, 0, 2, 25, 140};  // day (close)
    sprites[5] = {0, SpriteSize_32x32, SpriteColorFormat_256Color, 0, 2, 0, 20};    // day (far)
    // time
    sprites[6] = {0, SpriteSize_64x32, SpriteColorFormat_256Color, 0, 3, 9, -12};   // piece 0
    sprites[7] = {0, SpriteSize_64x32, SpriteColorFormat_256Color, 0, 4, 73, -12};  // piece 1
    sprites[8] = {0, SpriteSize_64x32, SpriteColorFormat_256Color, 0, 5, 137, -12}; // piece 2
    sprites[9] = {0, SpriteSize_64x32, SpriteColorFormat_256Color, 0, 6, 201, -12}; // piece 3
    // skill level
    sprites[10] = {0, SpriteSize_16x16, SpriteColorFormat_256Color, 0, 7, 86, 77};

	// initialize sub sprite engine with 1D mapping, 128 byte boundry, external palette support
	oamInit(&oamSub, SpriteMapping_1D_128, true);

	// allocating space for sprite graphics
    //moon
	sprites[0].gfx = oamAllocateGfx(&oamSub, SpriteSize_32x32, SpriteColorFormat_256Color);
    // day of the week
    sprites[1].gfx = oamAllocateGfx(&oamSub, SpriteSize_32x32, SpriteColorFormat_256Color);
    // numbers
    sprites[2].gfx = oamAllocateGfx(&oamSub, SpriteSize_32x32, SpriteColorFormat_256Color);
    sprites[3].gfx = oamAllocateGfx(&oamSub, SpriteSize_32x32, SpriteColorFormat_256Color);
    sprites[4].gfx = oamAllocateGfx(&oamSub, SpriteSize_32x32, SpriteColorFormat_256Color);
    sprites[5].gfx = oamAllocateGfx(&oamSub, SpriteSize_32x32, SpriteColorFormat_256Color);
    // time
    sprites[6].gfx = oamAllocateGfx(&oamSub, SpriteSize_64x32, SpriteColorFormat_256Color);
    sprites[7].gfx = oamAllocateGfx(&oamSub, SpriteSize_64x32, SpriteColorFormat_256Color);
    sprites[8].gfx = oamAllocateGfx(&oamSub, SpriteSize_64x32, SpriteColorFormat_256Color);
    sprites[9].gfx = oamAllocateGfx(&oamSub, SpriteSize_64x32, SpriteColorFormat_256Color);
    // skill level
    sprites[10].gfx = oamAllocateGfx(&oamSub, SpriteSize_16x16, SpriteColorFormat_256Color);

    // get sprites
    // moon
    spriteCtrl.switchSprite(MOON, MOON_22, &moonSprite);
    // day of the week
    spriteCtrl.switchSprite(DAY_OF_WEEK, MONDAY, &dayOfWeekSprite);
    // numbers
    spriteCtrl.switchSprite(DIGIT, DIGIT_0, &numberSprites[0]);
    spriteCtrl.switchSprite(DIGIT, DIGIT_1, &numberSprites[1]);
    spriteCtrl.switchSprite(DIGIT, DIGIT_3, &numberSprites[2]);
    spriteCtrl.switchSprite(DIGIT, DIGIT_0, &numberSprites[3]);
    // time
    spriteCtrl.switchSprite(TIME, AFTERNOON_0_0, &timeSprites[0]);
    spriteCtrl.switchSprite(TIME, AFTERNOON_1_0, &timeSprites[1]);
    spriteCtrl.switchSprite(TIME, AFTERNOON_2_0, &timeSprites[2]);
    spriteCtrl.switchSprite(TIME, AFTERNOON_0_0, &timeSprites[3]);
    // skill level
    spriteCtrl.switchSprite(SKILL_SPRITE, SKILLS_LEVEL, &skillSprites[0]);

    // TODO: initialize any extra sprite registers for max-case arrays?
    // ...

    // copy sprites
    // moon
	dmaCopy(moonSprite.tiles, sprites[0].gfx, moonSprite.tilesLen);
    // day of the week
    dmaCopy(dayOfWeekSprite.tiles, sprites[1].gfx, dayOfWeekSprite.tilesLen);
    // numbers
    dmaCopy(numberSprites[0].tiles, sprites[2].gfx, numberSprites[0].tilesLen);
    dmaCopy(numberSprites[1].tiles, sprites[3].gfx, numberSprites[1].tilesLen);
    dmaCopy(numberSprites[2].tiles, sprites[4].gfx, numberSprites[2].tilesLen);
    dmaCopy(numberSprites[3].tiles, sprites[5].gfx, numberSprites[3].tilesLen);
    // time
    dmaCopy(timeSprites[0].tiles, sprites[6].gfx, timeSprites[0].tilesLen);
    dmaCopy(timeSprites[1].tiles, sprites[7].gfx, timeSprites[1].tilesLen);
    dmaCopy(timeSprites[2].tiles, sprites[8].gfx, timeSprites[2].tilesLen);
    dmaCopy(timeSprites[3].tiles, sprites[9].gfx, timeSprites[3].tilesLen);
    // skill level
    dmaCopy(skillSprites[0].tiles, sprites[10].gfx, skillSprites[0].tilesLen);

    vramSetBankI(VRAM_I_LCD);
    dmaCopy(moonSprite.pal, &VRAM_I_EXT_SPR_PALETTE[0][0], moonSprite.palLen);              // moon
    dmaCopy(dayOfWeekSprite.pal, &VRAM_I_EXT_SPR_PALETTE[1][0], dayOfWeekSprite.palLen);    // day of the week
    dmaCopy(numberSprites[0].pal, &VRAM_I_EXT_SPR_PALETTE[2][0], numberSprites[0].palLen);  // numbers
    dmaCopy(timeSprites[0].pal, &VRAM_I_EXT_SPR_PALETTE[3][0], timeSprites[0].palLen);      // time (0)
    dmaCopy(timeSprites[1].pal, &VRAM_I_EXT_SPR_PALETTE[4][0], timeSprites[1].palLen);      // time (1)
    dmaCopy(timeSprites[2].pal, &VRAM_I_EXT_SPR_PALETTE[5][0], timeSprites[2].palLen);      // time (2)
    dmaCopy(timeSprites[3].pal, &VRAM_I_EXT_SPR_PALETTE[6][0], timeSprites[3].palLen);      // time (3)
    dmaCopy(skillSprites[0].pal, &VRAM_I_EXT_SPR_PALETTE[7][0], skillSprites[0].palLen);    // skill level
    vramSetBankI(VRAM_I_SUB_SPRITE_EXT_PALETTE);

    // setup player controller
    playerCtrl = new CharacterController(IWATODAI_DORM_MAP_WIDTH, IWATODAI_DORM_MAP_WIDTH, &iwatodai_dorm_map[0][0], tileSize, worldOffsetX, worldOffsetZ, characterSize, speed, angleIncrement, distance, lookAhead, angle, characterTranslate, characterFacingAngle);

    // setup music
    // 1/3 chance of a surprise
    if (rand() % 3 == 0) {
        musicCtrl.init("nitro:/music/toms_diner_suzanne_vega.pcm", 0.0f, 234.568f);
    } else {
        musicCtrl.init(IWATODAI_DORM_MUSIC, 0.0f, 920.973f);
    }

    // setup character model
    characterAnimationCtrl.loadModel("nitro:/models/character.bin");
    character_loadTextures(characterAnimationCtrl, bitmapsCharacter);

    // setup environment model
    const unsigned int* bitmapsEnv[IWATODAI_DORM_TEX_COUNT] = { textureBitmap };
    iwatodaiDormEnv.load("nitro:/environments/iwatodai_dorm.bin", bitmapsEnv);
    totalPolyCount = iwatodaiDormEnv.getPolyCount();

    // setup dialogue
    demo_dialogue_bg_slot = bgSharedSlot;

    // setup pause menu
    // use the same shared background slot as the demo dialogue
    pauseMenuCmpt.init(bgSharedSlot, &isPauseMenuActive);
}

ViewState IwatodaiDormView::Update()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    bgUpdate();

    scanKeys();
    u32 keys = keysHeld();
    u32 pressed = keysDown();

    if (pressed & KEY_START) {
        isPauseMenuActive = !isPauseMenuActive;
    }

    if (isPauseMenuActive) {
        ViewState menuResult = pauseMenuCmpt.update(pressed);
        if (menuResult != ViewState::KEEP_CURRENT) {
            musicCtrl.pause();
            return menuResult;
        }
    } else {    // only render world when pause menu is not active
        // only process world input when dialogue and battle are not active
        if (!dialogueCtrl.isActive() && !battleController.isActive()) {
            // move character
            camPos = playerCtrl->update(keys);

            // start battle
            if (keys & KEY_Y)
            {
                battleController.execute();
            }

            // trigger dialogue from interaction
            demo_unload();
            if (playerCtrl->isTileAt() == TileType::NEXT_SCENE) {
                musicCtrl.pause();
                return ViewState::IWATODAI_STREETS;
            } else if (playerCtrl->isTileAt() == TileType::CHARACTER_Akihiko) {
                iprintf("\x1b[0;0HPress A to talk");
                if (pressed & KEY_A) {
                    demo_yuki_guard_argument_load();
                    dialogueCtrl.setLoader(demo_yuki_guard_argument_load_bg);
                    dialogueCtrl.start(demo_yuki_guard_argument_first());
                }
            } else {
                consoleClear();
            }
        }

        // update camera position
        gluLookAt(camPos.cameraX, camPos.cameraY, camPos.cameraZ,
            camPos.targetX, camPos.targetY, camPos.targetZ,
            camPos.upX, camPos.upY, camPos.upZ);

        // draw sprites
        for (int i = 0; i < 11; i++)
        {
            oamSet(
                &oamSub,                    // sub display (OamState)
                i,                          // oam entry to set (id)
                sprites[i].x, sprites[i].y, // position
                1,                          // priority
                sprites[i].paletteAlpha,    // palette for 16 color sprite or alpha for bmp sprite
                sprites[i].size,
                sprites[i].format,
                sprites[i].gfx,
                sprites[i].rotationIndex,
                true,         // double the size of rotated sprites
                false,        // don't hide the sprite
                false, false, // vflip, hflip
                false         // apply mosaic
            );
        }

        // render multiple of the same sprits
        // in this case, the skills level
        // for (int i = 0; i < 3; i++)
        // {
        //     oamSet(
        //         &oamSub,                    // sub display (OamState)
        //         7 + i,                      // oam entry to set (id)
        //         sprites[7].x + (13 * i), sprites[7].y, // position
        //         1,                          // priority
        //         sprites[7].paletteAlpha,    // palette for 16 color sprite or alpha for bmp sprite
        //         sprites[7].size,
        //         sprites[7].format,
        //         sprites[7].gfx,
        //         sprites[7].rotationIndex,
        //         true,         // double the size of rotated sprites
        //         false,        // don't hide the sprite
        //         false, false, // vflip, hflip
        //         false         // apply mosaic
        //     );
        // }

        oamUpdate(&oamSub);

        // draw environment
        glPushMatrix();
            iwatodaiDormEnv.draw();
            iwatodaiDormEnv.drawBillboards(
                enableBillboards,  // billboards face camera
                camPos.cameraX, camPos.cameraY, camPos.cameraZ
            );
        glPopMatrix(1);

        // draw character
        glPushMatrix();
            // move character
            characterPosition charPos = playerCtrl->isCharacterAt();
            glTranslatef(charPos.x, 0.1, charPos.z);
            glRotatef(charPos.facingAngle, 0.0f, 1.0f, 0.0f);

            // draw character
            characterAnimationCtrl.render();
        glPopMatrix(1);

        glFlush(0);

        // print coordinates (64x64 area from 0,0 to 64,64)
        if (enableDebugPrint) {
            iprintf("\x1b[21;0Htile(x,z): %d, %d",
                (int)((charPos.x + worldOffsetX) / tileSize),
                (int)((charPos.z + worldOffsetZ) / tileSize));
            iprintf("\x1b[22;0Htranslate(x,z): %d, %d",
                (int)(charPos.x * 100),
                (int)(charPos.z * 100));
            iprintf("\x1b[23;0Hangle(w,c): %d, %d", (int)(charPos.angle * 100), (int)(charPos.facingAngle * 100));
        }
    }

    // update controllers
    battleController.update(pressed);
    dialogueCtrl.update(keys);
    characterAnimationCtrl.update();
    musicCtrl.update();

    return ViewState::KEEP_CURRENT;
}

void IwatodaiDormView::Cleanup()
{
    // clear screen
    setBrightness(3, 0);
    consoleClear();

    // stop pause menu sfx
    pauseMenuCmpt.cancelSFX();
    isPauseMenuActive = false;

    // cleanup environment
    iwatodaiDormEnv.cleanup();
    // reset textures
    glDeleteTextures(1, &characterTextureId);
    // reset shared bg slot
    dmaFillHalfWords(0, bgGetMapPtr(bgSharedSlot), 2048);

    // reset vram
    vramSetBankA(VRAM_A_LCD);
    vramSetBankB(VRAM_B_LCD);
    vramSetBankC(VRAM_C_LCD);
    vramSetBankD(VRAM_D_LCD);
    vramSetBankH(VRAM_H_LCD);

    // cleanup controllers
    delete playerCtrl;
    playerCtrl = NULL;
}
