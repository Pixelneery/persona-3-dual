#pragma once
#include "../BattleParticipant.h"
#include "../BattleResult.h"
#include "../skills/AttackSkill.h"
#include <nds.h>
#include <vector>

struct Enemy : BattleParticipant
{
    AttackSkill** attackSkill;
    u32 skillCount;
    BattleStats battleStats;

    Enemy()
    {
        participantType = ParticipantType::Enemy;
    }

    AttackSkill* pickSkill();
    BattleParticipant* pickTarget(std::vector<BattleParticipant*>& partyMembers);
    BattleResult resolve(BattleParticipant* target, AttackSkill* skill);

    virtual ~Enemy() = default;
};
