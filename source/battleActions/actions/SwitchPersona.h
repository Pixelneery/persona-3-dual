#pragma once
#include "ActionBase.h"

struct SwitchPersona : ActionBase
{
    SwitchPersona()
    {
        name = "SwitchPersona";
        possibleUsers = ParticipantType::Player;
    }

    BattleResult resolve(PartyMember *user, BattleParticipant *target, Skill *skill = nullptr) override;
};
