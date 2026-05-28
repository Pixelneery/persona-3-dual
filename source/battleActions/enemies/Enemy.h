#pragma once
#include <nds.h>
#include <vector>
#include "../skills/AttackSkill.h"
#include "../BattleParticipant.h"
#include "../BattleResult.h"

struct Enemy : BattleParticipant
{
    AttackSkill **attackSkill;
    u32 attackCount;
    BattleStats battleStats;

    Enemy()
    {
        participantType = ParticipantType::Enemy;
    }

    AttackSkill *pickSkill();
    BattleParticipant *pickTarget(std::vector<BattleParticipant *> &partyMembers);
    BattleResult resolve(BattleParticipant *target, AttackSkill *skill);

    virtual ~Enemy() = default;
};
