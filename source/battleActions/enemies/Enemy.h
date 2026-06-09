#pragma once
#include "../BattleParticipant.h"
#include "../BattleResult.h"
#include <nds.h>
#include <vector>

struct Enemy : BattleParticipant
{
    Skill** skill;
    u32 skillCount;
    BattleStats battleStats;

    Enemy()
    {
        participantType = ParticipantType::Enemy;
    }

    Skill* pickSkill();
    BattleParticipant* pickTarget(std::vector<BattleParticipant*>& partyMembers);
    BattleResult resolve(BattleParticipant* target, Skill* skill);

    BattleStats* getBattleStats() override
    {
        return &battleStats;
    }

    float calculateBaseDamage(BattleParticipant& defender, Skill& skill) override;
    float getTeamMultiplier() override;

    virtual ~Enemy() = default;
};
