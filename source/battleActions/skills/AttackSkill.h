#pragma once
#include <nds.h>
#include <cmath>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <algorithm>
#include "../Element.h"
#include "../BattleStats.h"
#include "../shoes/Shoe.h"
#include "Skill.h"

struct AttackSkill : Skill
{

    u32 calculateDamagePlayer(BattleStats *attackerStats, BattleStats *defenderStats, u32 *attackerLevel, u32 *defenderLevel)
    {
        damageSetup(attackerStats, defenderStats, attackerLevel, defenderLevel);
        // todo: level diffrence never goes under 0 for either party during boss fights
        return (u32)floor(sqrt((float)(movePower * 15 * Atk) / defenderStats->en) * 2 * levelDifference * affinityMtp);
    }

    // TODO: hopefully correct, should be looked at by someone that knows some  math
    u32 calculateHitratePlayer(BattleStats *attackerStats, BattleStats *defenderStats)
    {
        if (hitRate == 100)
            return true;

        float baseAccuracy = (float)(attackerStats->ag + 200) / (defenderStats->ag + 200);
        u32 multipliedAccuracy = (u32)(baseAccuracy * hitRate);
        return std::clamp(multipliedAccuracy, (u32)50, (u32)99);
    }

    // TODO: hopefully correct, should be looked at by someone that knows some  math
    u32 calculateHitrateEnemy(BattleStats *attackerStats, BattleStats *defenderStats, Shoe *shoe)
    {
        if (hitRate == 100)
            return true;

        float baseAccuracy = (float)(attackerStats->ag + 200) / (defenderStats->ag + 200);
        float shoeMultiplier = (float)(attackerStats->ag + 200) / (shoe->evasion / 2.0f + 200);
        u32 multipliedAccuracy = baseAccuracy * hitRate * shoeMultiplier;
        return std::clamp(multipliedAccuracy, (u32)50, (u32)99);
    }

    u32 calculateDamageEnemySkill(BattleStats *attackerStats, BattleStats *defenderStats, u32 *attackerLevel, u32 *defenderLevel, Armour *armour = nullptr)
    {
        // This value is used if an enemy attacks another enemy
        u32 armourValue = (armour != nullptr) ? armour->defense : 10;

        damageSetup(attackerStats, defenderStats, attackerLevel, defenderLevel);
        // todo: level diffrence never goes under 0 for either party during boss fights
        return (u32)floor((sqrt((float)(movePower * 6 * Atk) / (8 * defenderStats->en + armourValue)) * 9 * levelDifference - 10) * affinityMtp);
    }

    u32 calculateDamageEnemyRegular(BattleStats *attackerStats, BattleStats *defenderStats, u32 *attackerLevel, u32 *defenderLevel, Armour *armour = nullptr)
    {
        // This value is used if an enemy attacks another enemy
        u32 armourValue = (armour != nullptr) ? armour->defense : 10;

        damageSetup(attackerStats, defenderStats, attackerLevel, defenderLevel);
        // todo: level diffrence never goes under 0 for either party during boss fights
        return (u32)floor((sqrt((float)(movePower * 6 * Atk) / (8 * defenderStats->en + armourValue)) * 9 * levelDifference) * affinityMtp);
    }

    virtual ~AttackSkill() = default;

private:
    static const float levelMultipliers[24];
    u32 Atk;
    float affinityMtp = 1.0f;
    s32 diff;
    float levelDifference;

    void damageSetup(BattleStats *attackerStats, BattleStats *defenderStats, u32 *attackerLevel, u32 *defenderLevel);
};
