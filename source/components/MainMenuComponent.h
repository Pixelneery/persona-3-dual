#pragma once
#include "core/globals.h"
#include "core/BaseMenu.h"

#define MAIN_MENU_OPTIONS 3
#define LEVEL_OPTIONS 7
#define SETTING_OPTIONS 2
#define SETTING_INTRO_OPTIONS 4
#define SETTING_FEMC_OPTIONS 2

class MainMenuComponent : public BaseMenu
{
  protected:
    void loadBg(int bgIndex) override;

  private:
    MenuOption mainMenuOptions[MAIN_MENU_OPTIONS] = {
        {"Load Game", -1, MENU_BIND(MainMenuComponent, mainMenuOptionSelected)},
        {"Settings", -1, MENU_BIND(MainMenuComponent, mainMenuOptionSelected)},
        {"Return to Title", -1, MENU_BIND(MainMenuComponent, mainMenuOptionSelected)},
    };

    MenuOption levelOptions[LEVEL_OPTIONS] = {
        {"Start Game", -1, MENU_BIND(MainMenuComponent, levelOptionSelected)},
        {"Debug", -1, MENU_BIND(MainMenuComponent, levelOptionSelected)},
        {"Iwatodai Dorm", -1, MENU_BIND(MainMenuComponent, levelOptionSelected)},
        {"Iwatodai Streets", -1, MENU_BIND(MainMenuComponent, levelOptionSelected)},
        {"Station", -1, MENU_BIND(MainMenuComponent, levelOptionSelected)},
        {"Paulownia Mall", -1, MENU_BIND(MainMenuComponent, levelOptionSelected)},
        {"Sign Contract", -1, MENU_BIND(MainMenuComponent, levelOptionSelected)},
    };

    MenuOption settingOptions[SETTING_OPTIONS] = {
        {"Change Intro Video", -1, MENU_BIND(MainMenuComponent, settingOptionSelected)},
        {"Femc Mode",         -1, MENU_BIND(MainMenuComponent, settingOptionSelected)},
    };

    MenuOption settingFemcOptions[SETTING_FEMC_OPTIONS] =
    {
        {"Off", -1, MENU_BIND(MainMenuComponent, settingFemcOptionSelected)},
        {"On",  -1, MENU_BIND(MainMenuComponent, settingFemcOptionSelected)},
    };

    MenuOption settingIntroOptions[SETTING_INTRO_OPTIONS] = {
        {"Original", -1, MENU_BIND(MainMenuComponent, settingIntroOptionSelected)},
        {"FES", -1, MENU_BIND(MainMenuComponent, settingIntroOptionSelected)},
        {"Portable", -1, MENU_BIND(MainMenuComponent, settingIntroOptionSelected)},
        {"Reload", -1, MENU_BIND(MainMenuComponent, settingIntroOptionSelected)},
    };

    // option handlers
    ViewState mainMenuOptionSelected();
    ViewState levelOptionSelected();
    ViewState settingOptionSelected();
    ViewState settingIntroOptionSelected();
    ViewState settingFemcOptionSelected();
  public:
    void init(int iBgSlot, bool* isActive, const std::string& iPauseMessage = "") override;
};
