#include "BattleMenuComponent.h"

// dummy backgrounds
#include "bgAkihiko.h"
#include "bgKenji.h"
#include "bgYukari.h"
#include "bgYukariClose.h"

void BattleMenuComponent::loadBg(int bgIndex)
{
    // no background support yet
    return;
}

void BattleMenuComponent::init(int iBgSlot, bool *isActive, const std::string &iPauseMessage)
{
    BaseMenu::init(iBgSlot, isActive, iPauseMessage);
    options = nullptr;
    optionCount = 0;
}

// option loaders
void BattleMenuComponent::loadActionOptions(std::array<ActionBase*, 4>* actions)
{
    // skip if action options have already been loaded
    if (loadedOption == 1)
    {
        return;
    };

    // set new options
    battleOptions.clear();

    // indicate we loaded option
    loadedOption = 1;
    pauseMessage = "Actions";
    int count = actions->size();

    for (int i = 0; i < count; i++)
    {
        MenuOption option =
        {
            actions->at(i)->name.c_str(),
            -1,
            MENU_BIND(BattleMenuComponent, battleOptionSelected)
        };
        battleOptions.push_back(option);
    }

    options = battleOptions.data();
    optionCount = count;
}

void BattleMenuComponent::loadSkillOptions(PersonaBase* persona)
{
    // skip if action options have already been loaded
    if (loadedOption == 2)
    {
        return;
    };

    // set new options
    battleOptions.clear();

    // indicate we loaded option
    loadedOption = 2;
    pauseMessage = "Skills";
    int count = persona->attackCount;

    for (int i = 0; i < count; i++)
    {
        MenuOption option =
        {
            persona->skills[i]->name.c_str(),
            -1,
            MENU_BIND(BattleMenuComponent, battleOptionSelected)
        };
        battleOptions.push_back(option);
    }

    options = battleOptions.data();
    optionCount = count;
}

// option handlers
int BattleMenuComponent::battleOptionSelected()
{
    *isActivePtr = false;
    pauseMessage = "";
    return selectedOption;
}
