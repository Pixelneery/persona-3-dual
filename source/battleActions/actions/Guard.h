#pragma once
#include "ActionBase.h"
#include "../party/PartyMember.h"

struct Guard : ActionBase
{
    Guard()
    {
        name = "Guard";
        possibleUsers = ParticipantType::Party;
    }

    BattleResult resolve(PartyMember *user, BattleParticipant *target, Skill *skill = nullptr) override;
};
