#pragma once
#include "ActionBase.h"
#include "../enemies/Enemy.h"
#include "../party/PartyMember.h"

struct AttackAction : ActionBase {
    AttackAction() {
        name = "Attack";
        possibleUsers = ParticipantType::Party;
    }

    // Resolves a base-attack hit against one enemy target.
    // Applies damage directly, returns outcome for display.
    BattleResult resolve(PartyMember* user, BattleParticipant* target, Skill* skill = nullptr) override;
};
