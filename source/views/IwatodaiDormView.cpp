#include <nds.h>
#include <stdio.h>
#include "core/globals.h"
#include "math.h"
#include "IwatodaiDormView.h" 
#include "models/character_32x32.h"

// 3D models
#include "character.h"
// environment
#include "environments/iwatodai_dorm/texture.h"
// collision
#include "maps/iwatodaiDorm_0_0_44_33.h"
// dialogue
#include "dialogue/demo_dialogue.h"

// texture IDs
static int iwatodaiTextureIds[IWATODAI_DORM_TEX_COUNT];
static int characterTextureId;

AnimationController myCharacter;

void IwatodaiDormView::Init() {
    videoSetMode(MODE_0_3D);
    videoSetModeSub(MODE_0_2D);

    vramSetBankA(VRAM_A_TEXTURE);
    vramSetBankB(VRAM_B_TEXTURE);
    vramSetBankC(VRAM_C_SUB_BG);
    bgExtPaletteEnableSub();

    // main screen, 3D
    glInit();
    glEnable(GL_ANTIALIAS);
    glEnable(GL_TEXTURE_2D);    // enable texturing

    glClearColor(0,0,0,31);
    glClearPolyID(63);
    glClearDepth(0x7FFF);

    // set size of main screen
    glViewport(0,0,255,191);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // zNear is how close the camera can see, zFar is the maximum draw distance
    gluPerspective(55, 256.0 / 192.0, 0.1, 40);    

    // character
    glGenTextures(1, &characterTextureId);
    glBindTexture(GL_TEXTURE_2D, characterTextureId);
    glTexImage2D(
        GL_TEXTURE_2D, 0,
        GL_RGBA,
        TEXTURE_SIZE_32, TEXTURE_SIZE_32,
        0,
        TEXGEN_TEXCOORD | GL_TEXTURE_WRAP_S | GL_TEXTURE_WRAP_T,
        characterBitmap  // from character.h
    );

    glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK);
    glColor3b(255, 255, 255);   // keep white so texture colors aren't tinted

    // setup dialogue (sub screen)
    demo_dialogue_bg_slot = bgInitSub(0, BgType_Text8bpp, BgSize_T_256x256, 0, 1);
    dmaFillHalfWords(0, bgGetMapPtr(demo_dialogue_bg_slot), 2048);
    
    // setup console
    consoleInit(&console, 1, BgType_Text4bpp, BgSize_T_256x256, 4, 5, false, true);
    consoleSelect(&console);

    // adjust sub screen image and console to sit correctly on each other
    bgSetPriority(console.bgId, 0);
    bgSetPriority(demo_dialogue_bg_slot, 1);

    bgUpdate();

    // get controllers
    playerCtrl = new CharacterController(MAP_WIDTH, MAP_HEIGHT, &collision_map[0][0], tileSize, worldOffsetX, worldOffsetZ, characterRadius, speed, angleIncrement, distance, lookAhead, angle, translateX, translateZ, characterFacingAngle);

    // point to music
    musicCtrl.init("nitro:/music/changing_seasons.pcm", 0.0f, -1.0f);

    // setup character model
    LoadModel_character_32x32(myCharacter); 
    myCharacter.set(MODEL_CHARACTER_32X32_WALK, true);
    // TODO: play/pause animation on walk
    myCharacter.play();

    // setup environment model
    const unsigned int* bitmaps[IWATODAI_DORM_TEX_COUNT] = {
        textureBitmap
    };
    Load_iwatodai_dorm(iwatodaiTextureIds, bitmaps);
}

ViewState IwatodaiDormView::Update() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    bgUpdate();

    scanKeys();
    u32 keys = keysHeld();
    u32 pressed = keysDown();
    if(keys & KEY_START) {
        musicCtrl.pause();
        return ViewState::MAIN_MENU;
    }

    // only process world input when dialogue is not active
    if (!dialogueCtrl.isActive()) {
        // move character
        camPos = playerCtrl->update(keys);

        // trigger dialogue from interaction
        demo_unload();
        if (playerCtrl->isTileAt() == TileType::CHARACTER_Akihiko) {
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

    myCharacter.update();

    // update camera position
    gluLookAt(camPos.cameraX, camPos.cameraY, camPos.cameraZ,
        camPos.targetX, camPos.targetY, camPos.targetZ,
        camPos.upX, camPos.upY, camPos.upZ);

    // draw environment
    glPushMatrix();
        Draw_iwatodai_dorm(iwatodaiTextureIds);
    glPopMatrix(1);

    // draw character
    glPushMatrix();
        // move character
        characterPosition charPos = playerCtrl->isCharacterAt();
        glTranslatef(charPos.x, 0, charPos.z);
        glRotatef(charPos.facingAngle, 0.0f, 1.0f, 0.0f);
        // draw character
        glBindTexture(GL_TEXTURE_2D, characterTextureId);
        myCharacter.render();
    glPopMatrix(1);

    glFlush(0);

    // print coordinates (64x64 area from 0,0 to 64,64)
    iprintf("\x1b[21;0Htile(x,z): %d, %d",
        (int)((charPos.x + worldOffsetX) / tileSize),
        (int)((charPos.z + worldOffsetZ) / tileSize));
    iprintf("\x1b[22;0Htranslate(x,z): %d, %d",
        (int)(charPos.x * 100),
        (int)(charPos.z * 100));
    iprintf("\x1b[23;0Hangle(w,c): %d, %d", (int)(charPos.angle * 100), (int)(charPos.facingAngle * 100));

    // update controllers
    dialogueCtrl.update(keys);
    musicCtrl.update();

    return ViewState::KEEP_CURRENT;
}

void IwatodaiDormView::Cleanup() {
    // clear screen
    setBrightness(3, 0);
    consoleClear();

    // reset textures
    Delete_iwatodai_dorm(iwatodaiTextureIds);
    glDeleteTextures(1, &characterTextureId);

    // reset vram
    vramSetBankA(VRAM_A_LCD);
    vramSetBankB(VRAM_B_LCD);
    vramSetBankC(VRAM_C_LCD);
    vramSetBankH(VRAM_H_LCD);

    // cleanup controllers
    delete playerCtrl;
    playerCtrl = NULL;
}