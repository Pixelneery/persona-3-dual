#pragma once
#include "ActionBase.h"
#include "../enemies/Enemy.h"
#include "../party/PartyMember.h"

struct AttackAction : ActionBase
{
    AttackAction()
    {
        name = "Attack";
        possibleUsers = ParticipantType::Party;
    }

    BattleResult resolve(PartyMember *user, BattleParticipant *target, Skill *skill = nullptr) override;
};
