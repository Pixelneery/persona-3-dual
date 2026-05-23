#pragma once
#include "ActionBase.h"
#include "../enemies/Enemy.h"
#include "../party/PartyMember.h"
#include <stdio.h>
#include <vector>
#include "../UpdateIndex.h"
#include "../TargetAndExecute.h"

struct AttackAction : ActionBase
{
    UpdateIndex updateIndex;
    TargetAndExecute *targetAndExecute;

    AttackAction(std::vector<BattleParticipant *> *iAllParticipants, std::vector<BattleParticipant *> *iParty, std::vector<BattleParticipant *> *iEnemies) : ActionBase(iAllParticipants, iParty, iEnemies)
    {
        name = "AttackAction";
        possibleUsers = ParticipantType::Party;
        // TODO: dont forget to clear in the future
        targetAndExecute = new TargetAndExecute(&targetIndex);
    }

    void execute() override;
    bool update(u32 *keys, PartyMember *user) override;
};
