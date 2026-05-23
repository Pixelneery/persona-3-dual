#pragma once
#include <nds.h>
#include <cmath>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <algorithm>
#include "../BattleStats.h"
#include "../shoes/Shoe.h"
#include "Skill.h"

struct AttackSkill : Skill
{

    // TODO: potentially unify these functions (one calcdamage, internally decide correct formula)?
    u32 calculateDamagePlayer(BattleStats *attackerStats, BattleStats *defenderStats, u32 *attackerLevel, u32 *defenderLevel);
    u32 calculateHitratePlayer(BattleStats *attackerStats, BattleStats *defenderStats);
    u32 calculateHitrateEnemy(BattleStats *attackerStats, BattleStats *defenderStats, Shoe *shoe);
    u32 calculateDamageEnemySkill(BattleStats *attackerStats, BattleStats *defenderStats, u32 *attackerLevel, u32 *defenderLevel, Armour *armour = nullptr);
    u32 calculateDamageEnemyRegular(BattleStats *attackerStats, BattleStats *defenderStats, u32 *attackerLevel, u32 *defenderLevel, Armour *armour = nullptr);

    AttackSkill()
    {
        skillType = SkillType::Attack;
    }

private:
    static const float levelMultipliers[24];
    u32 Atk;
    float affinityMtp = 1.0f;
    s32 diff;
    float levelDifference;

    void damageSetup(BattleStats *attackerStats, BattleStats *defenderStats, u32 *attackerLevel, u32 *defenderLevel);
};
