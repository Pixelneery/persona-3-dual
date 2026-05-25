#pragma once
#include "ActionBase.h"
#include "../party/PartyMember.h"
#include "../enemies/Enemy.h"
#include "../skills/AttackSkill.h"
#include "../skills/HealSkill.h"

struct PersonaAction : ActionBase
{
    PersonaAction()
    {
        name = "Persona";
        possibleUsers = ParticipantType::Party;
    }

    BattleResult resolve(PartyMember *user, BattleParticipant *target, Skill *skill) override;
};
