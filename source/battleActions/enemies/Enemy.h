#pragma once
#include "../BattleParticipant.h"
#include "../BattleResult.h"
#include "../armours/EnemyArmour.h"
#include "../shoes/EnemyShoe.h"
#include <nds.h>
#include <vector>

struct Enemy : BattleParticipant
{
    Skill** skill;
    u32 skillCount;
    BattleStats battleStats;

    Enemy()
    {
        armour = new EnemyArmour;
        shoe = new EnemyShoe;
        participantType = ParticipantType::Enemy;
    }

    Skill* pickSkill();
    BattleParticipant* pickTarget(std::vector<BattleParticipant*>& partyMembers);
    BattleResult resolve(BattleParticipant* target, Skill* skill);

    BattleStats* getBattleStats() override
    {
        return &battleStats;
    }

    virtual ~Enemy() = default;
};
