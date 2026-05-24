#pragma once
#include "ActionBase.h"
#include "../party/PartyMember.h"

struct Guard : ActionBase {
    Guard() {
        name = "Guard";
        possibleUsers = ParticipantType::Party;
    }

    // Sets user->guarding = true. No target needed; pass nullptr.
    BattleResult resolve(PartyMember* user, BattleParticipant* target, Skill* skill = nullptr) override;
};
