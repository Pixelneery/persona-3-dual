#include "core/BaseMenu.h"

#define MAIN_MENU_OPTIONS 2
#define LEVEL_OPTIONS 3

class MainMenuComponent : public BaseMenu
{
protected:
    void loadBg(int bgIndex) override;
private:
    MenuOption mainMenuOptions[MAIN_MENU_OPTIONS] =
    {
        {"Load Game",       -1, MENU_BIND(MainMenuComponent, mainMenuOptionSelected)},
        {"Return to Title", -1, MENU_BIND(MainMenuComponent, mainMenuOptionSelected)},
    };

    MenuOption levelOptions[LEVEL_OPTIONS] =
    {
        {"Iwatodai Dorm",    -1, MENU_BIND(MainMenuComponent, levelOptionSelected)},
        {"Iwatodai Streets", -1, MENU_BIND(MainMenuComponent, levelOptionSelected)},
        {"Station",          -1, MENU_BIND(MainMenuComponent, levelOptionSelected)},
    };

    // option handlers
    ViewState mainMenuOptionSelected();
    ViewState levelOptionSelected();
public:
    void init(int iBgSlot, bool *isActive, const std::string &iPauseMessage = "") override;
};
