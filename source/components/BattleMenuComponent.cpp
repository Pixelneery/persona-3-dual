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
    optionCount = -1;
}

// option loaders
void BattleMenuComponent::loadActionOptions(std::array<ActionBase*, 4>* iActions)
{
    // skip if options have already been loaded
    if (loadedOption == 1)
    {
        return;
    };

    // set new options
    actionOptions.clear();
    actions = *iActions;

    // indicate we loaded option
    loadedOption = 1;
    pauseMessage = "Actions";
    int count = actions.size();

    for (int i = 0; i < count; i++)
    {
        MenuOption option =
        {
            actions[i]->name.c_str(),
            -1,
            MENU_BIND(BattleMenuComponent, actionOptionSelected)
        };
        actionOptions.push_back(option);
    }

    options = actionOptions.data();
    optionCount = count;
}

// option handlers
int BattleMenuComponent::actionOptionSelected()
{
    *isActivePtr = false;
    pauseMessage = "";
    return selectedOption;
}
