#pragma once
#include <nds.h>
#include <string>
#include "../BattleResult.h"
#include "../ParticipantType.h"

// Forward declarations
struct BattleParticipant;
struct PartyMember;
struct Skill;

struct ActionBase
{
    std::string name;
    ParticipantType possibleUsers;

    virtual BattleResult resolve(PartyMember *user, BattleParticipant *target, Skill *skill = nullptr) = 0;

    virtual ~ActionBase() = default;
};
