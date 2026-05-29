#include "MainMenuComponent.h"

// dummy backgrounds
#include "bgAkihiko.h"
#include "bgKenji.h"
#include "bgYukari.h"
#include "bgYukariClose.h"

void MainMenuComponent::loadBg(int bgIndex)
{
    if (bgIndex < 0)
        return;

    switch (bgIndex)
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

void MainMenuComponent::init(int iBgSlot, bool* isActive, const std::string& iPauseMessage)
{
    BaseMenu::init(iBgSlot, isActive, iPauseMessage);
    options = mainMenuOptions;
    optionCount = MAIN_MENU_OPTIONS;
}

// option handlers
ViewState MainMenuComponent::mainMenuOptionSelected()
{
    ViewState selectedView;
    switch (static_cast<MainMenuOptions>(selectedOption))
    {
    case MainMenuOptions::LOAD_GAME:
        changeMenu(levelOptions, LEVEL_OPTIONS);
        selectedView = ViewState::KEEP_CURRENT;
        break;
    case MainMenuOptions::SETTINGS:
        changeMenu(settingOptions, SETTING_OPTIONS);
        selectedView = ViewState::KEEP_CURRENT;
        break;
    case MainMenuOptions::RETURN_TO_TITLE:
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
    switch (static_cast<LevelOptions>(selectedOption))
    {
    case LevelOptions::START_GAME:
        selectedView = ViewState::CUTSCENE_1;
        break;
    case LevelOptions::DEBUG:
        selectedView = ViewState::DEBUG_VIEW;
        break;
    case LevelOptions::IWATODAI_DORM:
        selectedView = ViewState::IWATODAI_DORM;
        break;
    case LevelOptions::IWATODAI_STREETS:
        selectedView = ViewState::IWATODAI_STREETS;
        break;
    case LevelOptions::STATION:
        selectedView = ViewState::STATION;
        break;
    case LevelOptions::PAULOWNIA_MALL:
        selectedView = ViewState::PAULOWNIA_MALL;
        break;
    case LevelOptions::SIGN_CONTRACT:
        selectedView = ViewState::SIGN_CONTRACT;
        break;
    default:
        selectedView = ViewState::KEEP_CURRENT;
    }

    return selectedView;
}

ViewState MainMenuComponent::settingOptionSelected()
{
    ViewState selectedView;
    switch (static_cast<SettingOptions>(selectedOption))
    {
    case SettingOptions::CHANGE_INTRO_VIDEO:
        changeMenu(settingIntroOptions, SETTING_INTRO_OPTIONS);
        selectedView = ViewState::KEEP_CURRENT;
        break;
    case SettingOptions::FEMC_MODE:
        changeMenu(settingFemcOptions, SETTING_FEMC_OPTIONS);
        selectedView = ViewState::KEEP_CURRENT;
        break;
    default:
        selectedView = ViewState::KEEP_CURRENT;
    }

    return selectedView;
}

ViewState MainMenuComponent::settingFemcOptionSelected()
{
    switch (static_cast<SettingFemcOptions>(selectedOption))
    {
    case SettingFemcOptions::ON:
        saveData.femcMode = true;
        break;
    case SettingFemcOptions::OFF:
    default:
        saveData.femcMode = false;
        break;
    }
    return ViewState::KEEP_CURRENT;
}

ViewState MainMenuComponent::settingIntroOptionSelected()
{
    switch (static_cast<SettingIntroOptions>(selectedOption))
    {
    case SettingIntroOptions::ORIGINAL:
        saveData.introVideoPath = "base.vid";
        break;
    case SettingIntroOptions::FES:
        saveData.introVideoPath = "fes.vid";
        break;
    case SettingIntroOptions::PORTABLE:
        saveData.introVideoPath = "portable.vid";
        break;
    case SettingIntroOptions::RELOAD:
        saveData.introVideoPath = "reload.vid";
        break;
    default:
        saveData.introVideoPath = "reload.vid";
    }

    return ViewState::KEEP_CURRENT;
}
