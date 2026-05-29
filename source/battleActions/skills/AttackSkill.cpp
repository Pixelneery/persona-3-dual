#include "AttackSkill.h"

const float AttackSkill::levelMultipliers[24] = {0.5f,  0.51f, 0.53f, 0.59f, 0.66f, 0.75f, 0.84f, 0.91f,
                                                 0.97f, 0.99f, 1.0f,  1.0f,  1.0f,  1.0f,  1.01f, 1.03f,
                                                 1.09f, 1.16f, 1.25f, 1.34f, 1.41f, 1.47f, 1.49f, 1.5f};

u32 AttackSkill::calculateDamagePlayer(BattleStats* attackerStats,
                                       BattleStats* defenderStats,
                                       u32* attackerLevel,
                                       u32* defenderLevel)
{
    damageSetup(attackerStats, defenderStats, attackerLevel, defenderLevel);
    // todo: level diffrence never goes under 0 for either party during boss fights
    return (u32)floor(sqrt((float)(movePower * 15 * Atk) / defenderStats->en) * 2 * levelDifference * affinityMtp);
}

// TODO: hopefully correct, should be looked at by someone that knows some  math
u32 AttackSkill::calculateHitratePlayer(BattleStats* attackerStats, BattleStats* defenderStats)
{
    if (hitRate == 100)
        return true;

    float baseAccuracy = (float)(attackerStats->ag + 200) / (defenderStats->ag + 200);
    u32 multipliedAccuracy = (u32)(baseAccuracy * hitRate);
    return std::clamp(multipliedAccuracy, (u32)50, (u32)99);
}

u32 AttackSkill::calculateDamageEnemyRegular(
    BattleStats* attackerStats, BattleStats* defenderStats, u32* attackerLevel, u32* defenderLevel, Armour* armour)
{
    // This value is used if an enemy attacks another enemy
    u32 armourValue = (armour != nullptr) ? armour->defense : 10;

    damageSetup(attackerStats, defenderStats, attackerLevel, defenderLevel);
    // todo: level diffrence never goes under 0 for either party during boss fights
    return (u32)floor(
        (sqrt((float)(movePower * 6 * Atk) / (8 * defenderStats->en + armourValue)) * 9 * levelDifference) *
        affinityMtp);
}

u32 AttackSkill::calculateDamageEnemySkill(
    BattleStats* attackerStats, BattleStats* defenderStats, u32* attackerLevel, u32* defenderLevel, Armour* armour)
{
    // This value is used if an enemy attacks another enemy
    u32 armourValue = (armour != nullptr) ? armour->defense : 10;

    damageSetup(attackerStats, defenderStats, attackerLevel, defenderLevel);
    // todo: level diffrence never goes under 0 for either party during boss fights

    return (u32)floor(
        (sqrt((float)(movePower * 6 * Atk) / (8 * defenderStats->en + armourValue)) * 9 * levelDifference - 10) *
        affinityMtp);
}

// TODO: hopefully correct, should be looked at by someone that knows some  math
u32 AttackSkill::calculateHitrateEnemy(BattleStats* attackerStats, BattleStats* defenderStats, Shoe* shoe)
{
    if (hitRate == 100)
        return true;

    float baseAccuracy = (float)(attackerStats->ag + 200) / (defenderStats->ag + 200);
    float shoeMultiplier = (float)(attackerStats->ag + 200) / (shoe->evasion / 2.0f + 200);
    u32 multipliedAccuracy = baseAccuracy * hitRate * shoeMultiplier;
    return std::clamp(multipliedAccuracy, (u32)50, (u32)99);
}

void AttackSkill::damageSetup(BattleStats* attackerStats,
                              BattleStats* defenderStats,
                              u32* attackerLevel,
                              u32* defenderLevel)
{
    if (skillRace == SkillRace::phys)
        Atk = attackerStats->st;
    else
        Atk = attackerStats->ma;

    // TODO: std clamp
    diff = *attackerLevel - *defenderLevel;
    if (diff < -13)
        diff = -13;
    else if (diff > 10)
        diff = 10;

    levelDifference = levelMultipliers[diff + 13]; // offset so -13 is 0

    u32 affinity = defenderStats->affinities[element];

    if (affinity == BattleStats::Affinity::Weak)
        affinityMtp = 1.25f;
    else if (affinity == BattleStats::Affinity::Neutral)
        affinityMtp = 1.0f;
    else if (affinity == BattleStats::Affinity::Resist)
        affinityMtp = 0.5f;
    else if (affinity == BattleStats::Affinity::Null)
        affinityMtp = 0.0f;
    else if (affinity == BattleStats::Affinity::Absorb)
        affinityMtp = -1.0f;
    // TODO: add repel logic
    else if (affinity == BattleStats::Affinity::Repel)
        affinityMtp = -2.0f;
}
