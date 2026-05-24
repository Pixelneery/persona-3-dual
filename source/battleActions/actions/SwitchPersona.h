#pragma once
#include "ActionBase.h"

struct SwitchPersona : ActionBase {
    SwitchPersona() {
        name = "SwitchPersona";
        possibleUsers = ParticipantType::Player;
    }

    // No-op: BattleController handles persona switching directly in ChoosePersona phase.
    BattleResult resolve(PartyMember* user, BattleParticipant* target, Skill* skill = nullptr) override;
};
