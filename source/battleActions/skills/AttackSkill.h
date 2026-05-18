#pragma once
#include <nds.h>
#include <cmath>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include "../Element.h"
#include "../BattleStats.h"

struct AttackSkill
{
    float moveDamage;
    enum Race
    {
        phys,
        mag
    };

    s32 cost;
    Race race;
    u32 element;
    u32 hitRate;
    std::string name;

    u32 calculateDamagePlayer(BattleStats *attackerStats, BattleStats *defenderStats, u32 *attackerLevel, u32 *defenderLevel)
    {
        damageSetup(attackerStats, defenderStats, attackerLevel, defenderLevel);
        // todo: level diffrence never goes under 0 for either party during boss fights
        return (u32)floor(sqrt((float)(moveDamage * 15 * Atk) / defenderStats->en) * 2 * levelDifference * affinityMtp);
    }

    u32 calculateDamageEnemySkill(BattleStats *attackerStats, BattleStats *defenderStats, u32 *attackerLevel, u32 *defenderLevel, Armour *armour = nullptr)
    {
        // This value is used if an enemy attacks another enemy
        u32 armourValue = (armour != nullptr) ? armour->defense : 10;

        damageSetup(attackerStats, defenderStats, attackerLevel, defenderLevel);
        // todo: level diffrence never goes under 0 for either party during boss fights
        return (u32)floor((sqrt((float)(moveDamage * 6 * Atk) / (8 * defenderStats->en + armourValue)) * 9 * levelDifference - 10) * affinityMtp);
    }

    u32 calculateDamageEnemyRegular(BattleStats *attackerStats, BattleStats *defenderStats, u32 *attackerLevel, u32 *defenderLevel, Armour *armour = nullptr)
    {
        // This value is used if an enemy attacks another enemy
        u32 armourValue = (armour != nullptr) ? armour->defense : 10;

        damageSetup(attackerStats, defenderStats, attackerLevel, defenderLevel);
        // todo: level diffrence never goes under 0 for either party during boss fights
        return (u32)floor((sqrt((float)(moveDamage * 6 * Atk) / (8 * defenderStats->en + armourValue)) * 9 * levelDifference) * affinityMtp);
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
