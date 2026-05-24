#pragma once
#include "ActionBase.h"
#include "../party/PartyMember.h"
#include "../enemies/Enemy.h"
#include "../skills/AttackSkill.h"
#include "../skills/HealSkill.h"

struct PersonaAction : ActionBase {
    PersonaAction() {
        name = "Persona";
        possibleUsers = ParticipantType::Party;
    }

    // skill must be non-null. Cost must already be deducted by BattleController before calling.
    BattleResult resolve(PartyMember* user, BattleParticipant* target, Skill* skill) override;
};
