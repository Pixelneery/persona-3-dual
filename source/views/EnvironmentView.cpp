#include "EnvironmentView.h"
#include "core/globals.h"
#include <nds.h>
#include <string>

// model
#include "models/kotone.h"
#include "models/makoto.h"
// demo dialogue
#include "dialogue/demo_dialogue.h"

namespace
{
/**
 * @brief Loads a single .grit asset and returns its raw tile pointer.
 *
 * Stashes the owning GraphicAsset in @p asset so the caller can unload it
 * once the texture has been uploaded to VRAM.
 *
 * @param path  Full path (base path + grit base name) of the asset to load.
 * @param asset Output parameter that receives the loaded GraphicAsset,
 *              which the caller is responsible for unloading later.
 * @return Raw pointer to the asset's tile data, reinterpreted as
 *         unsigned int, suitable for passing to the texture upload code.
 */
const unsigned int* loadEnvironmentBitmap(const std::string& path, GraphicAsset& asset)
{
    asset = GraphicsController::getInstance()->loadGrit(path);
    return reinterpret_cast<const unsigned int*>(asset.tiles);
}

/**
 * @brief Strips the compiled ".img.bin" suffix from a texture filename to
 *        recover the base name expected by loadGrit.
 *
 * environmentDb.cpp stores the *compiled* texture filename, e.g.
 * "f007_002wall01.img.bin", but loadGrit wants the .grit base name instead
 * (e.g. "f007_002wall01").
 *
 * @param compiledFileName The compiled texture filename as stored in the
 *                          environment database (e.g. "name.img.bin").
 * @return The same name with a trailing ".img.bin" suffix removed, or the
 *         name unchanged if it does not end with that suffix.
 */
std::string gritBaseName(const char* compiledFileName)
{
    std::string name(compiledFileName);
    static const std::string suffix = ".img.bin";
    if (name.size() > suffix.size() && name.compare(name.size() - suffix.size(), suffix.size(), suffix) == 0)
    {
        name.erase(name.size() - suffix.size());
    }
    return name;
}
} // namespace

void EnvironmentView::setupEnvironment()
{
    GraphicAsset envTextures[MAX_ENVIRONMENT_TEXTURES] = {};
    const unsigned int* bitmapsEnv[MAX_ENVIRONMENT_TEXTURES] = {nullptr};

    const std::string basePath = fatBasePath + "environments/" + dbEntry->name + "/";

    for (int i = 0; i < dbEntry->textureCount; ++i)
    {
        bitmapsEnv[i] = loadEnvironmentBitmap(basePath + gritBaseName(dbEntry->textures[i].name), envTextures[i]);
    }

    if (!env.load(dbEntry, bitmapsEnv))
    {
        iprintf("EnvironmentView: failed to load environment '%s'\n", dbEntry->name);
    }

    for (int i = 0; i < dbEntry->textureCount; ++i)
    {
        graphicsCtrl->unloadGrit(envTextures[i]);
    }
}

void EnvironmentView::init()
{
    BaseView3D::init();

    dbEntry = getEnvironmentDbEntry();
    if (!dbEntry)
    {
        iprintf("EnvironmentView::init - no EnvironmentDbEntry for this room\n");
        return;
    }

    // setup sub screen
    // https://mtheall.com/vram.html#SUB=1&T0=1&NT0=512&MB0=2&TB0=1&S0=0&T1=3&NT1=128&MB1=5&TB1=0&T2=1&NT2=512&MB2=3&TB2=3&S2=0&T3=1&NT3=512&MB3=4&TB3=5&S3=0
    bgSharedSub1 = bgInitSub(0, BgType_Text8bpp, BgSize_T_256x256, 2, 1);
    bgSharedSub2 = bgInitSub(2, BgType_Text8bpp, BgSize_T_256x256, 3, 3);
    bgSharedSub3 = bgInitSub(3, BgType_Text8bpp, BgSize_T_256x256, 4, 5);

    dmaFillHalfWords(0, bgGetMapPtr(bgSharedSub1), 2048);
    dmaFillHalfWords(0, bgGetMapPtr(bgSharedSub2), 2048);
    dmaFillHalfWords(0, bgGetMapPtr(bgSharedSub3), 2048);

    // setup console
    consoleInit(&console, 1, BgType_Text4bpp, BgSize_T_256x256, 5, 0, false, true);
    consoleSelect(&console);

    // adjust sub screen image and console to sit correctly on each other
    bgSetPriority(console.bgId, 0);
    bgSetPriority(bgSharedSub1, 1);
    bgSetPriority(bgSharedSub2, 2);
    bgSetPriority(bgSharedSub3, 3);
    bgUpdate();

    // setup player controller (room-specific map/tuning, generic call site)
    playerCtrl = createPlayerController();

    // setup music (room-specific path/loop points)
    setMusic();

    // setup character model (identical across rooms)
    std::string modelPath = fatBasePath + "models/";
    characterAnimationCtrl->loadModel(
        (modelPath + (saveData.femcMode ? "kotone/kotone.bin" : "makoto/makoto.bin")).c_str());

    if (saveData.femcMode)
    {
        kotone_loadTextures(*characterAnimationCtrl, (const unsigned int**)bitmapsCharacter);
    }
    else
    {
        makoto_loadTextures(*characterAnimationCtrl, (const unsigned int**)bitmapsCharacter);
    }

    // setup environment geometry/textures (fully generic, data-driven)
    setupEnvironment();

    // setup dialogue rendering target (which sub-bg the dialogue box uses)
    demo_dialogue_bg_slot = bgSharedSub1;

    // setup pause menu
    pauseMenuCmpt->init(bgSharedSub1);

    // setup battle menu
    battleMenuCmpt->init(-1, &isBattleMenuActive);

    // setup UI
    // NOTE: bg 0 is the 3D view
    int bgMain[3] = {1, 2, 3};
    // TODO: Setting the first index to anything other than bgSharedSub results in black bg (but sprites still load)
    // This might be okay/intended, as long as we create 4 seperate bg to pass in
    int bgSub[4] = {bgSharedSub2, bgSharedSub3, 2, 3};

    // initialize sub sprite engine with 1D mapping, 128 byte boundry, external palette support
    oamInit(&oamSub, SpriteMapping_1D_128, true);

    uiCtrl->setGraphics(bgSub, bgMain, &oamSub, nullptr);
    uiCtrl->registerScreen(menuHUDScreen, false);
    uiCtrl->registerScreen(dialogueScreen, false);
    uiCtrl->show(menuHUDScreen, false);

    onSetupDialogueAndUI();

    // setup view phases
    prevPauseState = false;
    prevDialogueState = false;
    prevEnvironmentState = false;
    isBattleMenuActive = false;
    prevBattleState = false;
    phase = ViewPhase::Environment;
}

ViewState EnvironmentView::update()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    bgUpdate();
    oamUpdate(&oamSub);

    scanKeys();

    u32 keys = keysHeld();
    u32 pressed = keysDown();

    switch (phase)
    {
    case ViewPhase::Battle:
    {
        if (!prevBattleState)
        {
            uiCtrl->hideAll();

            startBattle();

            prevBattleState = true;
        }

        battleController->update(pressed);

        if (!battleController->isActive() && prevBattleState)
        {
            prevBattleState = false;

            uiCtrl->show(menuHUDScreen, false);

            prevEnvironmentState = true;
            phase = ViewPhase::Environment;

            setMusic();
        }

        break;
    }

    case ViewPhase::Pause:
    {
        if (!prevPauseState)
        {
            uiCtrl->hideAll();
            prevPauseState = true;
        }

        ViewState menuResult = pauseMenuCmpt->update(pressed);

        if (menuResult != ViewState::KEEP_CURRENT)
        {
            musicCtrl->pause();
            return menuResult;
        }

        if (pressed & KEY_START)
        {
            consoleClear();

            prevPauseState = false;
            phase = ViewPhase::Environment;
            prevEnvironmentState = false;
        }

        break;
    }

    case ViewPhase::Dialogue:
    {
        bool isActive = dialogueCtrl.isActive();

        if (!isActive && !prevDialogueState)
        {
            uiCtrl->show(dialogueScreen, false);

            onDialogueStart();

            prevDialogueState = true;
        }
        else if (!isActive && prevDialogueState)
        {
            bgHide(bgSharedSub1);

            prevDialogueState = false;
            prevEnvironmentState = false;

            phase = ViewPhase::Environment;
        }

        dialogueCtrl.update(keys);

        break;
    }

    case ViewPhase::Environment:
    {
        if (!prevEnvironmentState)
        {
            uiCtrl->show(menuHUDScreen, false);
            prevEnvironmentState = true;
        }

        camPos = playerCtrl->update(keys);

        if (pressed & KEY_START)
        {
            prevEnvironmentState = false;
            phase = ViewPhase::Pause;
            break;
        }

        if (pressed & KEY_TOUCH)
        {
            touchRead(&touch);

            if (menuHUDScreen->onTouch(&touch) == 1)
            {
                prevEnvironmentState = false;
                phase = ViewPhase::Pause;
                break;
            }
        }

        ViewState tileResult = onTileCheck(playerCtrl->isTileAt(), pressed);

        if (tileResult != ViewState::KEEP_CURRENT)
        {
            musicCtrl->pause();
            return tileResult;
        }

        gluLookAt(camPos.cameraX,
                  camPos.cameraY + getCameraYOffset(),
                  camPos.cameraZ,
                  camPos.targetX,
                  camPos.targetY,
                  camPos.targetZ,
                  camPos.upX,
                  camPos.upY,
                  camPos.upZ);

        glPushMatrix();

        env.draw();

        env.drawBillboards(Globals::enableBillboards, camPos.cameraX, camPos.cameraY, camPos.cameraZ);

        glPopMatrix(1);

        glPushMatrix();

        CharacterPosition charPos = playerCtrl->isCharacterAt();

        glTranslatef(charPos.x, charPos.y, charPos.z);

        glRotatef(charPos.facingAngle, 0.0f, 1.0f, 0.0f);

        characterAnimationCtrl->render();

        glPopMatrix(1);

        glFlush(0);

        if (Globals::enableDebugPrint)
        {
            iprintf("\x1b[19;0H\033[31mTouch x = %04X, %04X\n", touch.rawx, touch.px);

            iprintf("\x1b[20;0HTouch y = %04X, %04X\n", touch.rawy, touch.py);

            iprintf("\x1b[21;0Htile(x,z): %d, %d",
                    (int)((charPos.x + dbEntry->worldOffsetX) / tileSize),
                    (int)((charPos.z + dbEntry->worldOffsetZ) / tileSize));

            iprintf("\x1b[22;0Htranslate(x,z): %d, %d", (int)(charPos.x * 100), (int)(charPos.z * 100));

            iprintf("\x1b[23;0Hangle(w,c): %d, %d \033[37;1m",
                    (int)(charPos.angle * 100),
                    (int)(charPos.facingAngle * 100));
        }

        break;
    }

    default:
    {
        phase = ViewPhase::Environment;
        break;
    }
    }

    characterAnimationCtrl->update();
    musicCtrl->update();

    return ViewState::KEEP_CURRENT;
}

void EnvironmentView::cleanup()
{
    BaseView::cleanup();

    env.cleanup();
    uiCtrl->cleanup();

    delete playerCtrl;
    playerCtrl = nullptr;
}
