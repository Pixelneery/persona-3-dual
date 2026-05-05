#include <nds.h>
#include "demo_dialogue.h"

int demo_dialogue_bg_slot = 0;

// background import
#include "bgAkihiko.h"
#include "bgGuard.h"
#include "bgYuki.h"
#include "bgYukiClose.h"

void demo_unload() {
    bgHide(demo_dialogue_bg_slot);
}

const char* demo_yuki_guard_argument_bg_names[4] = { "bgAkihiko", "bgGuard", "bgYuki", "bgYukiClose" };
void (*demo_yuki_guard_argument_bg_loaders[4])() = {
    nullptr,
    nullptr,
    nullptr,
    nullptr,
};

void demo_yuki_guard_argument_load_bg(int bgIndex) {
    if (bgIndex >= 0 && bgIndex < 4 && demo_yuki_guard_argument_bg_loaders[bgIndex])
        demo_yuki_guard_argument_bg_loaders[bgIndex]();
}

void demo_yuki_guard_argument_load() {
    demo_yuki_guard_argument_bg_loaders[0] = [](){
        dmaCopy(bgAkihikoTiles, bgGetGfxPtr(demo_dialogue_bg_slot), bgAkihikoTilesLen);
        dmaCopy(bgAkihikoMap, bgGetMapPtr(demo_dialogue_bg_slot), bgAkihikoMapLen);
        vramSetBankH(VRAM_H_LCD);
        dmaCopy(bgAkihikoPal, &VRAM_H_EXT_PALETTE[0][0], bgAkihikoPalLen);
        vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);
        bgShow(demo_dialogue_bg_slot);
    };
    demo_yuki_guard_argument_bg_loaders[1] = [](){
        dmaCopy(bgGuardTiles, bgGetGfxPtr(demo_dialogue_bg_slot), bgGuardTilesLen);
        dmaCopy(bgGuardMap, bgGetMapPtr(demo_dialogue_bg_slot), bgGuardMapLen);
        vramSetBankH(VRAM_H_LCD);
        dmaCopy(bgGuardPal, &VRAM_H_EXT_PALETTE[0][0], bgGuardPalLen);
        vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);
        bgShow(demo_dialogue_bg_slot);
    };
    demo_yuki_guard_argument_bg_loaders[2] = [](){
        dmaCopy(bgYukiTiles, bgGetGfxPtr(demo_dialogue_bg_slot), bgYukiTilesLen);
        dmaCopy(bgYukiMap, bgGetMapPtr(demo_dialogue_bg_slot), bgYukiMapLen);
        vramSetBankH(VRAM_H_LCD);
        dmaCopy(bgYukiPal, &VRAM_H_EXT_PALETTE[0][0], bgYukiPalLen);
        vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);
        bgShow(demo_dialogue_bg_slot);
    };
    demo_yuki_guard_argument_bg_loaders[3] = [](){
        dmaCopy(bgYukiCloseTiles, bgGetGfxPtr(demo_dialogue_bg_slot), bgYukiCloseTilesLen);
        dmaCopy(bgYukiCloseMap, bgGetMapPtr(demo_dialogue_bg_slot), bgYukiCloseMapLen);
        vramSetBankH(VRAM_H_LCD);
        dmaCopy(bgYukiClosePal, &VRAM_H_EXT_PALETTE[0][0], bgYukiClosePalLen);
        vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);
        bgShow(demo_dialogue_bg_slot);
    };
    demo_yuki_guard_argument_init();
}

dialogue demo_yuki_guard_argument_lines[18];

void demo_yuki_guard_argument_init() {
    dialogueSelection demo_yuki_guard_argument_sel_6_0 = { "Vouch for Yuki", false, &demo_yuki_guard_argument_lines[7] };
    dialogueSelection demo_yuki_guard_argument_sel_6_1 = { "Side with the guard", false, &demo_yuki_guard_argument_lines[12] };
    dialogueSelection demo_yuki_guard_argument_sel_6_2 = { "Stay out of it", false, &demo_yuki_guard_argument_lines[15] };

    demo_yuki_guard_argument_lines[0] = { "Guard", "Ma'am, the east path is closed  after sundown. I've said it     twice. ", 0, NULL, &demo_yuki_guard_argument_lines[1], {} };
    demo_yuki_guard_argument_lines[1] = { "Yuki", "And I've told you - I left my   bag on the bench. It will take  thirty seconds. ", 1, &demo_yuki_guard_argument_lines[0], &demo_yuki_guard_argument_lines[2], {} };
    demo_yuki_guard_argument_lines[2] = { "Guard", "Rules are rules. Come back      tomorrow. ", 0, &demo_yuki_guard_argument_lines[1], &demo_yuki_guard_argument_lines[3], {} };
    demo_yuki_guard_argument_lines[3] = { "Akihiko", "Hey, what's going on? ", 2, &demo_yuki_guard_argument_lines[2], &demo_yuki_guard_argument_lines[4], {} };
    demo_yuki_guard_argument_lines[4] = { "Yuki", "This guard won't let me through to get my bag. ", 1, &demo_yuki_guard_argument_lines[3], &demo_yuki_guard_argument_lines[5], {} };
    demo_yuki_guard_argument_lines[5] = { "Guard", "Sir, please stay back. This     doesn't concern you. ", 0, &demo_yuki_guard_argument_lines[4], &demo_yuki_guard_argument_lines[6], {} };
    demo_yuki_guard_argument_lines[6] = { "Akihiko", "Can we work something out here? ", 2, &demo_yuki_guard_argument_lines[5], NULL, { demo_yuki_guard_argument_sel_6_0, demo_yuki_guard_argument_sel_6_1, demo_yuki_guard_argument_sel_6_2 } };
    demo_yuki_guard_argument_lines[7] = { "Akihiko", "I know her. She's not going to  cause any trouble. ", 2, &demo_yuki_guard_argument_lines[6], &demo_yuki_guard_argument_lines[8], {} };
    demo_yuki_guard_argument_lines[8] = { "Guard", "... ", 0, &demo_yuki_guard_argument_lines[7], &demo_yuki_guard_argument_lines[9], {} };
    demo_yuki_guard_argument_lines[9] = { "Guard", "Fine. Two minutes. And you're   coming with her. ", 0, &demo_yuki_guard_argument_lines[8], &demo_yuki_guard_argument_lines[10], {} };
    demo_yuki_guard_argument_lines[10] = { "Yuki", "Thank you. ", 1, &demo_yuki_guard_argument_lines[9], &demo_yuki_guard_argument_lines[11], {} };
    demo_yuki_guard_argument_lines[11] = { "Yuki", "...Both of you. ", 3, &demo_yuki_guard_argument_lines[10], NULL, {} };
    demo_yuki_guard_argument_lines[12] = { "Akihiko", "He's got a point, Yuki. Come    back in the morning. ", 2, &demo_yuki_guard_argument_lines[11], &demo_yuki_guard_argument_lines[13], {} };
    demo_yuki_guard_argument_lines[13] = { "Yuki", "Are you serious right now? ", 1, &demo_yuki_guard_argument_lines[12], &demo_yuki_guard_argument_lines[14], {} };
    demo_yuki_guard_argument_lines[14] = { "Yuki", "Fine. Don't talk to me tonight. ", 3, &demo_yuki_guard_argument_lines[13], NULL, {} };
    demo_yuki_guard_argument_lines[15] = { "Akihiko", "Actually - yeah, none of my     business. Carry on. ", 2, &demo_yuki_guard_argument_lines[14], &demo_yuki_guard_argument_lines[16], {} };
    demo_yuki_guard_argument_lines[16] = { "Guard", "Smart man. ", 0, &demo_yuki_guard_argument_lines[15], &demo_yuki_guard_argument_lines[17], {} };
    demo_yuki_guard_argument_lines[17] = { "Yuki", "Unbelievable. ", 1, &demo_yuki_guard_argument_lines[16], NULL, {} };
}
