#pragma once
// #include "core/globals.h"
#include "core/BaseMenu.h"
#include "controllers/BattleController.h"

class BattleMenuComponent : public BaseMenu
{
protected:
    void loadBg(int bgIndex) override;
private:
    int loadedOption = 0;   // 1 = actionOptions
    std::array<ActionBase*, 4> actions;

    std::vector<MenuOption> actionOptions;

    // option handlers
    int actionOptionSelected();
public:
    void init(int iBgSlot, bool *isActive, const std::string &iPauseMessage = "") override;
    // option loaders
    void loadActionOptions(std::array<ActionBase*, 4>* iActions);
};
