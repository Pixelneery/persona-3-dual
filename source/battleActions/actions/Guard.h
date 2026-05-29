#pragma once
#include "../party/PartyMember.h"
#include "ActionBase.h"

struct Guard : ActionBase
{
    Guard()
    {
        name = "Guard";
        possibleUsers = ParticipantType::Party;
    }

    BattleResult resolve(PartyMember* user, BattleParticipant* target, Skill* skill = nullptr) override;
};
