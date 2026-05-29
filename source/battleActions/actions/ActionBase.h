#pragma once
#include "../BattleResult.h"
#include "../ParticipantType.h"
#include <nds.h>
#include <string>

// Forward declarations
struct BattleParticipant;
struct PartyMember;
struct Skill;

struct ActionBase
{
    std::string name;
    ParticipantType possibleUsers;

    virtual BattleResult resolve(PartyMember* user, BattleParticipant* target, Skill* skill = nullptr) = 0;

    virtual ~ActionBase() = default;
};
