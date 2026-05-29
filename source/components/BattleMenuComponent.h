#pragma once
// #include "core/globals.h"
#include "controllers/BattleController.h"
#include "core/BaseMenu.h"

class BattleMenuComponent : public BaseMenu
{
  protected:
    void loadBg(int bgIndex) override;

  private:
    // TODO: create enum
    BattleMenuOptions loadedOption = BattleMenuOptions::NONE;

    std::vector<MenuOption> battleOptions;
    int alertStartFrame = 0;

    // option handlers
    int battleOptionSelected();

  public:
    void init(int iBgSlot, bool* isActive, const std::string& iPauseMessage = "") override;
    ViewState update(int keys) override;
    // option loaders
    void loadActionOptions(std::array<ActionBase*, 4>* actions, std::string name);
    void loadSkillOptions(PersonaBase* persona);
    void loadPersonaOptions(std::vector<PersonaBase*>* personas);
    void loadTargetOptions(std::vector<BattleParticipant*>* targets, bool healTarget);
    void loadAlertOptions(const std::string& text);
    bool isAlertExpired(int durationFrames) const;
    void reset();
};
