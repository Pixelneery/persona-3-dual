#include "_project_assets_dialogue_demo_dlg_dialogue.h"
#include "controllers/GraphicsController.h"
#include "core/globals.h"
#include <nds.h>

int _project_assets_dialogue_demo_dlg_dialogue_bg_slot = 0;

const char* _project_assets_dialogue_demo_dlg_yukari_kenji_argument_bg_names[4] = {
    "bgAkihiko", "bgKenji", "bgYukari", "bgYukariClose"};
void (*_project_assets_dialogue_demo_dlg_yukari_kenji_argument_bg_loaders[4])() = {
    nullptr,
    nullptr,
    nullptr,
    nullptr,
};

void _project_assets_dialogue_demo_dlg_yukari_kenji_argument_load_bg(int bgIndex)
{
    if (bgIndex >= 0 && bgIndex < 4 && _project_assets_dialogue_demo_dlg_yukari_kenji_argument_bg_loaders[bgIndex])
    {
        _project_assets_dialogue_demo_dlg_yukari_kenji_argument_bg_loaders[bgIndex]();
    }
}

void _project_assets_dialogue_demo_dlg_yukari_kenji_argument_load()
{
    _project_assets_dialogue_demo_dlg_yukari_kenji_argument_bg_loaders[0] = []()
    {
        GraphicAsset bg =
            GraphicsController::getInstance()->loadGrit(fatBasePath + "graphics/dialogue/backgrounds/bgAkihiko");
        if (bg.tiles)
        {
            dmaCopy(bg.tiles, bgGetGfxPtr(_project_assets_dialogue_demo_dlg_dialogue_bg_slot), bg.tilesLen);
            dmaCopy(bg.map, bgGetMapPtr(_project_assets_dialogue_demo_dlg_dialogue_bg_slot), bg.mapLen);
            vramSetBankH(VRAM_H_LCD);
            dmaCopy(bg.pal, &VRAM_H_EXT_PALETTE[0][0], bg.palLen);
            vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);
            bgShow(_project_assets_dialogue_demo_dlg_dialogue_bg_slot);
            GraphicsController::getInstance()->unloadGrit(bg);
        }
    };
    _project_assets_dialogue_demo_dlg_yukari_kenji_argument_bg_loaders[1] = []()
    {
        GraphicAsset bg =
            GraphicsController::getInstance()->loadGrit(fatBasePath + "graphics/dialogue/backgrounds/bgKenji");
        if (bg.tiles)
        {
            dmaCopy(bg.tiles, bgGetGfxPtr(_project_assets_dialogue_demo_dlg_dialogue_bg_slot), bg.tilesLen);
            dmaCopy(bg.map, bgGetMapPtr(_project_assets_dialogue_demo_dlg_dialogue_bg_slot), bg.mapLen);
            vramSetBankH(VRAM_H_LCD);
            dmaCopy(bg.pal, &VRAM_H_EXT_PALETTE[0][0], bg.palLen);
            vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);
            bgShow(_project_assets_dialogue_demo_dlg_dialogue_bg_slot);
            GraphicsController::getInstance()->unloadGrit(bg);
        }
    };
    _project_assets_dialogue_demo_dlg_yukari_kenji_argument_bg_loaders[2] = []()
    {
        GraphicAsset bg =
            GraphicsController::getInstance()->loadGrit(fatBasePath + "graphics/dialogue/backgrounds/bgYukari");
        if (bg.tiles)
        {
            dmaCopy(bg.tiles, bgGetGfxPtr(_project_assets_dialogue_demo_dlg_dialogue_bg_slot), bg.tilesLen);
            dmaCopy(bg.map, bgGetMapPtr(_project_assets_dialogue_demo_dlg_dialogue_bg_slot), bg.mapLen);
            vramSetBankH(VRAM_H_LCD);
            dmaCopy(bg.pal, &VRAM_H_EXT_PALETTE[0][0], bg.palLen);
            vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);
            bgShow(_project_assets_dialogue_demo_dlg_dialogue_bg_slot);
            GraphicsController::getInstance()->unloadGrit(bg);
        }
    };
    _project_assets_dialogue_demo_dlg_yukari_kenji_argument_bg_loaders[3] = []()
    {
        GraphicAsset bg =
            GraphicsController::getInstance()->loadGrit(fatBasePath + "graphics/dialogue/backgrounds/bgYukariClose");
        if (bg.tiles)
        {
            dmaCopy(bg.tiles, bgGetGfxPtr(_project_assets_dialogue_demo_dlg_dialogue_bg_slot), bg.tilesLen);
            dmaCopy(bg.map, bgGetMapPtr(_project_assets_dialogue_demo_dlg_dialogue_bg_slot), bg.mapLen);
            vramSetBankH(VRAM_H_LCD);
            dmaCopy(bg.pal, &VRAM_H_EXT_PALETTE[0][0], bg.palLen);
            vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);
            bgShow(_project_assets_dialogue_demo_dlg_dialogue_bg_slot);
            GraphicsController::getInstance()->unloadGrit(bg);
        }
    };
    _project_assets_dialogue_demo_dlg_yukari_kenji_argument_init();
}

Dialogue _project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[18];

void _project_assets_dialogue_demo_dlg_yukari_kenji_argument_init()
{
    DialogueSelection _project_assets_dialogue_demo_dlg_yukari_kenji_argument_sel_6_0 = {
        "Vouch for Yukari", false, &_project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[7]};
    DialogueSelection _project_assets_dialogue_demo_dlg_yukari_kenji_argument_sel_6_1 = {
        "Side with Kenji", false, &_project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[12]};
    DialogueSelection _project_assets_dialogue_demo_dlg_yukari_kenji_argument_sel_6_2 = {
        "Stay out of it", false, &_project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[15]};

    _project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[0] = {
        "Kenji",
        "Yukari, the east path is closed after sundown. I've said it     twice. ",
        1,
        NULL,
        &_project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[1],
        {}};
    _project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[1] = {
        "Yukari",
        "And I've told you - I left my   bag on the bench. It will take  thirty seconds. ",
        2,
        &_project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[0],
        &_project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[2],
        {}};
    _project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[2] = {
        "Kenji",
        "Rules are rules. Come back      tomorrow. ",
        1,
        &_project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[1],
        &_project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[3],
        {}};
    _project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[3] = {
        "Akihiko",
        "Hey, what's going on? ",
        0,
        &_project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[2],
        &_project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[4],
        {}};
    _project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[4] = {
        "Yukari",
        "Kenji won't let me through to   get my bag. ",
        2,
        &_project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[3],
        &_project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[5],
        {}};
    _project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[5] = {
        "Kenji",
        "Akihiko, stay back. This doesn't concern you. ",
        1,
        &_project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[4],
        &_project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[6],
        {}};
    _project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[6] = {
        "Akihiko",
        "Can we work something out here? ",
        0,
        &_project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[5],
        NULL,
        {_project_assets_dialogue_demo_dlg_yukari_kenji_argument_sel_6_0,
         _project_assets_dialogue_demo_dlg_yukari_kenji_argument_sel_6_1,
         _project_assets_dialogue_demo_dlg_yukari_kenji_argument_sel_6_2}};
    _project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[7] = {
        "Akihiko",
        "C'mon, she's not going to cause any trouble. ",
        0,
        &_project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[6],
        &_project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[8],
        {}};
    _project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[8] = {
        "Kenji",
        "... ",
        1,
        &_project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[7],
        &_project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[9],
        {}};
    _project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[9] = {
        "Kenji",
        "Fine. Two minutes. And you're   coming with her. ",
        1,
        &_project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[8],
        &_project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[10],
        {}};
    _project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[10] = {
        "Yukari",
        "Thank you. ",
        2,
        &_project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[9],
        &_project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[11],
        {}};
    _project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[11] = {
        "Yukari", "...Both of you. ", 3, &_project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[10], NULL, {}};
    _project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[12] = {
        "Akihiko",
        "He's got a point, Yukari. Come  back in the morning. ",
        0,
        &_project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[11],
        &_project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[13],
        {}};
    _project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[13] = {
        "Yukari",
        "Are you serious right now? ",
        2,
        &_project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[12],
        &_project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[14],
        {}};
    _project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[14] = {
        "Yukari",
        "Fine. Don't talk to me tonight. ",
        3,
        &_project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[13],
        NULL,
        {}};
    _project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[15] = {
        "Akihiko",
        "Actually - yeah, none of my     business. Carry on. ",
        0,
        &_project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[14],
        &_project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[16],
        {}};
    _project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[16] = {
        "Kenji",
        "Smart man. ",
        1,
        &_project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[15],
        &_project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[17],
        {}};
    _project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[17] = {
        "Yukari", "Unbelievable. ", 2, &_project_assets_dialogue_demo_dlg_yukari_kenji_argument_lines[16], NULL, {}};
}
