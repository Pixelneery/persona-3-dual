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
    int loadedOption = 0;   // 1 = actionOptions, 2 = skillOptions, 3 = personaOptions, 4 = targetOptions (enemies), 5 = targetOptions (heal)

    std::vector<MenuOption> battleOptions;

    // option handlers
    int battleOptionSelected();
public:
    void init(int iBgSlot, bool *isActive, const std::string &iPauseMessage = "") override;
    // option loaders
    void loadActionOptions(std::array<ActionBase*, 4>* actions, std::string name);
    void loadSkillOptions(PersonaBase* persona);
    void loadPersonaOptions(std::vector<PersonaBase*>* personas);
    void loadTargetOptions(std::vector<BattleParticipant*>* targets, bool healTarget);
};
