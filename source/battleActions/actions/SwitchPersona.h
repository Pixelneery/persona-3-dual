#pragma once
#include <stdio.h>

#include "ActionBase.h"
#include "../party/PartyMember.h"
#include "../UpdateIndex.h"

struct SwitchPersona : ActionBase
{
    UpdateIndex updateIndex;

    SwitchPersona(std::vector<BattleParticipant *> *iAllParticipants, std::vector<BattleParticipant *> *iParty, std::vector<BattleParticipant *> *iEnemies) : ActionBase(iAllParticipants, iParty, iEnemies)
    {
        name = "SwitchPersona";
        possibleUsers = ParticipantType::Player;
    }

    void execute() override;
    bool update(u32 *keys, PartyMember *user) override;
};
