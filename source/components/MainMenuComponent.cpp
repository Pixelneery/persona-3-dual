#include "MainMenuComponent.h"

// dummy backgrounds
#include "bgAkihiko.h"
#include "bgKenji.h"
#include "bgYukari.h"
#include "bgYukariClose.h"

void MainMenuComponent::loadBg(int bgIndex)
{
    if (bgIndex < 0) return;

    switch(bgIndex)
    {
        case 0: // Akihiko
            dmaCopy(bgAkihikoTiles, bgGetGfxPtr(bgSlot), bgAkihikoTilesLen);
            dmaCopy(bgAkihikoMap, bgGetMapPtr(bgSlot), bgAkihikoMapLen);
            vramSetBankH(VRAM_H_LCD);
            dmaCopy(bgAkihikoPal, &VRAM_H_EXT_PALETTE[0][0], bgAkihikoPalLen);
            vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);
            break;

        case 1: // Kenji
            dmaCopy(bgKenjiTiles, bgGetGfxPtr(bgSlot), bgKenjiTilesLen);
            dmaCopy(bgKenjiMap, bgGetMapPtr(bgSlot), bgKenjiMapLen);
            vramSetBankH(VRAM_H_LCD);
            dmaCopy(bgKenjiPal, &VRAM_H_EXT_PALETTE[0][0], bgKenjiPalLen);
            vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);
            break;

        case 2: // Yukari
            dmaCopy(bgYukariTiles, bgGetGfxPtr(bgSlot), bgYukariTilesLen);
            dmaCopy(bgYukariMap, bgGetMapPtr(bgSlot), bgYukariMapLen);
            vramSetBankH(VRAM_H_LCD);
            dmaCopy(bgYukariPal, &VRAM_H_EXT_PALETTE[0][0], bgYukariPalLen);
            vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);
            bgShow(bgSlot);
            break;

        case 3: // YukariClose
            dmaCopy(bgYukariCloseTiles, bgGetGfxPtr(bgSlot), bgYukariCloseTilesLen);
            dmaCopy(bgYukariCloseMap, bgGetMapPtr(bgSlot), bgYukariCloseMapLen);
            vramSetBankH(VRAM_H_LCD);
            dmaCopy(bgYukariClosePal, &VRAM_H_EXT_PALETTE[0][0], bgYukariClosePalLen);
            vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);
            bgShow(bgSlot);
            break;

        default:
            break;
    }

}

void MainMenuComponent::init(int iBgSlot, bool *isActive, const std::string &iPauseMessage)
{
    BaseMenu::init(iBgSlot, isActive, iPauseMessage);
    options = mainMenuOptions;
    optionCount = MAIN_MENU_OPTIONS;
}

// option handlers
ViewState MainMenuComponent::mainMenuOptionSelected()
{
    ViewState selectedView;
    switch (selectedOption)
    {
        case (int)MainMenuOptions::LOAD_GAME:
            changeMenu(levelOptions, LEVEL_OPTIONS);
            selectedView = ViewState::KEEP_CURRENT;
            break;
        case (int)MainMenuOptions::RETURN_TO_TITLE:
            selectedView = ViewState::INTRO;
            break;
        default:
            selectedView = ViewState::KEEP_CURRENT;
    }

    return selectedView;
}

ViewState MainMenuComponent::levelOptionSelected()
{
    ViewState selectedView;
    switch (selectedOption)
    {
        case (int)LevelOptions::IWATODAI_DORM:
            selectedView = ViewState::IWATODAI_DORM;
            break;
        case (int)LevelOptions::IWATODAI_STREETS:
            selectedView = ViewState::IWATODAI_STREETS;
            break;
        default:
            selectedView = ViewState::KEEP_CURRENT;
    }

    return selectedView;
}
