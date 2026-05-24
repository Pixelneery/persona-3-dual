#pragma once
#include <nds.h>
#include <string>
#include "../BattleResult.h"
#include "../ParticipantType.h"

// Forward declarations
struct BattleParticipant;
struct PartyMember;
struct Skill;

struct ActionBase {
    std::string name;
    ParticipantType possibleUsers;

    // Resolve the action for the given actor and target.
    // skill is provided for PersonaAction; other actions may ignore it.
    // Side effects (HP change, knockedDown) are applied directly inside resolve().
    // The returned BattleResult carries the outcome for display/logging.
    virtual BattleResult resolve(PartyMember* user, BattleParticipant* target, Skill* skill = nullptr) = 0;

    virtual ~ActionBase() = default;
};
