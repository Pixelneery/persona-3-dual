#pragma once
// #include "core/globals.h"
#include "core/BaseMenu.h"
#include "controllers/BattleController.h"

class BattleMenuComponent : public BaseMenu
{
protected:
    void loadBg(int bgIndex) override;
private:
    // TODO: create enum
    int loadedOption = 0;   // 1 = actionOptions, 2 = skillOptions

    std::vector<MenuOption> battleOptions;

    // option handlers
    int battleOptionSelected();
public:
    void init(int iBgSlot, bool *isActive, const std::string &iPauseMessage = "") override;
    // option loaders
    void loadActionOptions(std::array<ActionBase*, 4>* actions);
    void loadSkillOptions(PersonaBase* persona);
};
