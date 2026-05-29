#include "demo_dialogue.h"
#include <nds.h>

int demo_dialogue_bg_slot = 0;

// background import
#include "bgAkihiko.h"
#include "bgKenji.h"
#include "bgYukari.h"
#include "bgYukariClose.h"

const char* demo_yukari_kenji_argument_bg_names[4] = {"bgAkihiko", "bgKenji", "bgYukari", "bgYukariClose"};
void (*demo_yukari_kenji_argument_bg_loaders[4])() = {
    nullptr,
    nullptr,
    nullptr,
    nullptr,
};

void demo_yukari_kenji_argument_load_bg(int bgIndex)
{
    if (bgIndex >= 0 && bgIndex < 4 && demo_yukari_kenji_argument_bg_loaders[bgIndex])
        demo_yukari_kenji_argument_bg_loaders[bgIndex]();
}

void demo_yukari_kenji_argument_load()
{
    demo_yukari_kenji_argument_bg_loaders[0] = []()
    {
        dmaCopy(bgAkihikoTiles, bgGetGfxPtr(demo_dialogue_bg_slot), bgAkihikoTilesLen);
        dmaCopy(bgAkihikoMap, bgGetMapPtr(demo_dialogue_bg_slot), bgAkihikoMapLen);
        vramSetBankH(VRAM_H_LCD);
        dmaCopy(bgAkihikoPal, &VRAM_H_EXT_PALETTE[0][0], bgAkihikoPalLen);
        vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);
        bgShow(demo_dialogue_bg_slot);
    };
    demo_yukari_kenji_argument_bg_loaders[1] = []()
    {
        dmaCopy(bgKenjiTiles, bgGetGfxPtr(demo_dialogue_bg_slot), bgKenjiTilesLen);
        dmaCopy(bgKenjiMap, bgGetMapPtr(demo_dialogue_bg_slot), bgKenjiMapLen);
        vramSetBankH(VRAM_H_LCD);
        dmaCopy(bgKenjiPal, &VRAM_H_EXT_PALETTE[0][0], bgKenjiPalLen);
        vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);
        bgShow(demo_dialogue_bg_slot);
    };
    demo_yukari_kenji_argument_bg_loaders[2] = []()
    {
        dmaCopy(bgYukariTiles, bgGetGfxPtr(demo_dialogue_bg_slot), bgYukariTilesLen);
        dmaCopy(bgYukariMap, bgGetMapPtr(demo_dialogue_bg_slot), bgYukariMapLen);
        vramSetBankH(VRAM_H_LCD);
        dmaCopy(bgYukariPal, &VRAM_H_EXT_PALETTE[0][0], bgYukariPalLen);
        vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);
        bgShow(demo_dialogue_bg_slot);
    };
    demo_yukari_kenji_argument_bg_loaders[3] = []()
    {
        dmaCopy(bgYukariCloseTiles, bgGetGfxPtr(demo_dialogue_bg_slot), bgYukariCloseTilesLen);
        dmaCopy(bgYukariCloseMap, bgGetMapPtr(demo_dialogue_bg_slot), bgYukariCloseMapLen);
        vramSetBankH(VRAM_H_LCD);
        dmaCopy(bgYukariClosePal, &VRAM_H_EXT_PALETTE[0][0], bgYukariClosePalLen);
        vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);
        bgShow(demo_dialogue_bg_slot);
    };
    demo_yukari_kenji_argument_init();
}

dialogue demo_yukari_kenji_argument_lines[18];

void demo_yukari_kenji_argument_init()
{
    dialogueSelection demo_yukari_kenji_argument_sel_6_0 = {
        "Vouch for Yukari", false, &demo_yukari_kenji_argument_lines[7]};
    dialogueSelection demo_yukari_kenji_argument_sel_6_1 = {
        "Side with Kenji", false, &demo_yukari_kenji_argument_lines[12]};
    dialogueSelection demo_yukari_kenji_argument_sel_6_2 = {
        "Stay out of it", false, &demo_yukari_kenji_argument_lines[15]};

    demo_yukari_kenji_argument_lines[0] = {"Kenji",
                                           "Yukari, the east path is closed after sundown. I've said it     twice. ",
                                           1,
                                           NULL,
                                           &demo_yukari_kenji_argument_lines[1],
                                           {}};
    demo_yukari_kenji_argument_lines[1] = {
        "Yukari",
        "And I've told you - I left my   bag on the bench. It will take  thirty seconds. ",
        2,
        &demo_yukari_kenji_argument_lines[0],
        &demo_yukari_kenji_argument_lines[2],
        {}};
    demo_yukari_kenji_argument_lines[2] = {"Kenji",
                                           "Rules are rules. Come back      tomorrow. ",
                                           1,
                                           &demo_yukari_kenji_argument_lines[1],
                                           &demo_yukari_kenji_argument_lines[3],
                                           {}};
    demo_yukari_kenji_argument_lines[3] = {"Akihiko",
                                           "Hey, what's going on? ",
                                           0,
                                           &demo_yukari_kenji_argument_lines[2],
                                           &demo_yukari_kenji_argument_lines[4],
                                           {}};
    demo_yukari_kenji_argument_lines[4] = {"Yukari",
                                           "Kenji won't let me through to   get my bag. ",
                                           2,
                                           &demo_yukari_kenji_argument_lines[3],
                                           &demo_yukari_kenji_argument_lines[5],
                                           {}};
    demo_yukari_kenji_argument_lines[5] = {"Kenji",
                                           "Akihiko, stay back. This doesn't concern you. ",
                                           1,
                                           &demo_yukari_kenji_argument_lines[4],
                                           &demo_yukari_kenji_argument_lines[6],
                                           {}};
    demo_yukari_kenji_argument_lines[6] = {
        "Akihiko",
        "Can we work something out here? ",
        0,
        &demo_yukari_kenji_argument_lines[5],
        NULL,
        {demo_yukari_kenji_argument_sel_6_0, demo_yukari_kenji_argument_sel_6_1, demo_yukari_kenji_argument_sel_6_2}};
    demo_yukari_kenji_argument_lines[7] = {"Akihiko",
                                           "C'mon, she's not going to cause any trouble. ",
                                           0,
                                           &demo_yukari_kenji_argument_lines[6],
                                           &demo_yukari_kenji_argument_lines[8],
                                           {}};
    demo_yukari_kenji_argument_lines[8] = {
        "Kenji", "... ", 1, &demo_yukari_kenji_argument_lines[7], &demo_yukari_kenji_argument_lines[9], {}};
    demo_yukari_kenji_argument_lines[9] = {"Kenji",
                                           "Fine. Two minutes. And you're   coming with her. ",
                                           1,
                                           &demo_yukari_kenji_argument_lines[8],
                                           &demo_yukari_kenji_argument_lines[10],
                                           {}};
    demo_yukari_kenji_argument_lines[10] = {
        "Yukari", "Thank you. ", 2, &demo_yukari_kenji_argument_lines[9], &demo_yukari_kenji_argument_lines[11], {}};
    demo_yukari_kenji_argument_lines[11] = {
        "Yukari", "...Both of you. ", 3, &demo_yukari_kenji_argument_lines[10], NULL, {}};
    demo_yukari_kenji_argument_lines[12] = {"Akihiko",
                                            "He's got a point, Yukari. Come  back in the morning. ",
                                            0,
                                            &demo_yukari_kenji_argument_lines[11],
                                            &demo_yukari_kenji_argument_lines[13],
                                            {}};
    demo_yukari_kenji_argument_lines[13] = {"Yukari",
                                            "Are you serious right now? ",
                                            2,
                                            &demo_yukari_kenji_argument_lines[12],
                                            &demo_yukari_kenji_argument_lines[14],
                                            {}};
    demo_yukari_kenji_argument_lines[14] = {
        "Yukari", "Fine. Don't talk to me tonight. ", 3, &demo_yukari_kenji_argument_lines[13], NULL, {}};
    demo_yukari_kenji_argument_lines[15] = {"Akihiko",
                                            "Actually - yeah, none of my     business. Carry on. ",
                                            0,
                                            &demo_yukari_kenji_argument_lines[14],
                                            &demo_yukari_kenji_argument_lines[16],
                                            {}};
    demo_yukari_kenji_argument_lines[16] = {
        "Kenji", "Smart man. ", 1, &demo_yukari_kenji_argument_lines[15], &demo_yukari_kenji_argument_lines[17], {}};
    demo_yukari_kenji_argument_lines[17] = {
        "Yukari", "Unbelievable. ", 2, &demo_yukari_kenji_argument_lines[16], NULL, {}};
}
