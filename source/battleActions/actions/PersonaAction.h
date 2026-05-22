#pragma once
#include <stdio.h>
#include <vector>

#include "ActionBase.h"
#include "../party/PartyMember.h"
#include "../TargetAndExecute.h"
#include "../UpdateIndex.h"

struct PersonaAction : ActionBase
{
    UpdateIndex updateIndex;
    AttackSkill *selectedSkill;
    TargetAndExecute *targetAndExecute;

    PersonaAction(std::vector<BattleParticipant *> *iAllParticipants, std::vector<BattleParticipant *> *iParty, std::vector<BattleParticipant *> *iEnemies) : ActionBase(iAllParticipants, iParty, iEnemies)
    {
        name = "Persona";
        possibleUsers = ParticipantType::Party;

        // TODO: dont forget to clear in the future
        targetAndExecute = new TargetAndExecute(enemies, &targetIndex);
    }

    enum MenuState
    {
        SelectSkill,
        SelectTarget,
    };
    MenuState menuState;

    void execute() override;
    bool update(u32 *keys, PartyMember *user) override;
};
