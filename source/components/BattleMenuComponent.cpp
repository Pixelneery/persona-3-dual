#include "BattleMenuComponent.h"
#include "core/globals.h"

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
void BattleMenuComponent::loadActionOptions(std::array<ActionBase*, 4>* actions, std::string name)
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
    pauseMessage = name.c_str();
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

void BattleMenuComponent::loadPersonaOptions(std::vector<PersonaBase*>* personas)
{
    if (loadedOption == 3)
        return;

    battleOptions.clear();
    loadedOption = 3;
    pauseMessage = "Persona";
    int count = (int)personas->size();

    for (int i = 0; i < count; i++)
    {
        MenuOption option =
        {
            personas->at(i)->name.c_str(),
            -1,
            MENU_BIND(BattleMenuComponent, battleOptionSelected)
        };
        battleOptions.push_back(option);
    }

    options = battleOptions.data();
    optionCount = count;
}

void BattleMenuComponent::loadTargetOptions(std::vector<BattleParticipant*>* targets, bool healTarget)
{
    int targetLoadedOption = healTarget ? 5 : 4;
    if (loadedOption == targetLoadedOption)
        return;

    battleOptions.clear();
    loadedOption = targetLoadedOption;
    pauseMessage = "Target";
    int count = (int)targets->size();

    for (int i = 0; i < count; i++)
    {
        MenuOption option =
        {
            targets->at(i)->name.c_str(),
            -1,
            MENU_BIND(BattleMenuComponent, battleOptionSelected)
        };
        battleOptions.push_back(option);
    }

    options = battleOptions.data();
    optionCount = count;
}

void BattleMenuComponent::loadAlertOptions(const std::string& text)
{
    if (loadedOption == 6)
        return;

    battleOptions.clear();
    loadedOption = 6;
    pauseMessage = text;
    optionCount = 0;
    alertStartFrame = frame;
}

bool BattleMenuComponent::isAlertExpired(int durationFrames) const
{
    return (frame - alertStartFrame) >= durationFrames;
}

void BattleMenuComponent::reset()
{
    loadedOption = 0;
    selectedOption = 0;
    startIndex = 0;
}

ViewState BattleMenuComponent::update(int keys)
{
    if (loadedOption == 6)
    {
        consoleClear();
        iprintf("\x1b[0;0H");
        iprintf("%s", pauseMessage.c_str());
        return ViewState::KEEP_CURRENT;
    }
    return BaseMenu::update(keys);
}

// option handlers
int BattleMenuComponent::battleOptionSelected()
{
    *isActivePtr = false;
    pauseMessage = "";
    return selectedOption;
}
