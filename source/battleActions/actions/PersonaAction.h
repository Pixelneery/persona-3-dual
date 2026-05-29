#pragma once
#include "../enemies/Enemy.h"
#include "../party/PartyMember.h"
#include "../skills/AttackSkill.h"
#include "../skills/HealSkill.h"
#include "ActionBase.h"

struct PersonaAction : ActionBase
{
    PersonaAction()
    {
        name = "Persona";
        possibleUsers = ParticipantType::Party;
    }

    BattleResult resolve(PartyMember* user, BattleParticipant* target, Skill* skill) override;
};
