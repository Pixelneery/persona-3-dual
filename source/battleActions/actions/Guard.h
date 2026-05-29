#pragma once
#include "ActionBase.h"
#include "../party/PartyMember.h"

struct Guard : ActionBase
{
    Guard(std::vector<BattleParticipant *> *iAllParticipants, std::vector<BattleParticipant *> *iParty, std::vector<BattleParticipant *> *iEnemies) : ActionBase(iAllParticipants, iParty, iEnemies)
    {
        name = "Guard";
        possibleUsers = ParticipantType::Party;
    }

    void execute() override;
    bool update(u32 *keys, PartyMember *user) override;
};
