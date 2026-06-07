#include "BattleCalcs.h"
#include "../enemies/Enemy.h"
#include "../party/PartyMember.h"

u32 BattleCalcs::Atk = 0;
float BattleCalcs::affinityMtp = 0;
s32 BattleCalcs::diff = 0;
float BattleCalcs::levelDifference = 0;

u32 BattleCalcs::attack(BattleParticipant& attacker, BattleParticipant& defender, Skill& skill)
{
    BattleStats& attackerStats = *attacker.getBattleStats();
    BattleStats& defenderStats = *defender.getBattleStats();
    BattleCalcs::damageSetup(attackerStats, defenderStats, attacker.lv, defender.lv, skill);
    float base = 0;
    if (attacker.participantType == ParticipantType::Enemy)
    {
        if (skill.skillType == SkillType::RegularAttack)
            base = (sqrt((float)(skill.movePower * 6 * Atk) / (8 * defenderStats.en + defender.armour->defense)) * 9 *
                    levelDifference) *
                   affinityMtp;
        else if (skill.skillType == SkillType::Attack || skill.skillType == SkillType::MultiAttack)
            base = ((sqrt((float)(skill.movePower * 6 * Atk) / (8 * defenderStats.en + defender.armour->defense)) * 9 *
                         levelDifference -
                     10) *
                    affinityMtp);
    }
    else
        base = floor(sqrt((float)(skill.movePower * 15 * Atk) / defenderStats.en) * 2 * levelDifference * affinityMtp);

    float range = 95 + (u32)(rand() % 11);
    return std::clamp((u32)trunc(base * range / 100.0f), (u32)1, (u32)99999);
}

u32 BattleCalcs::hitrate(BattleParticipant& attacker, BattleParticipant& defender, Skill& skill)
{
    BattleStats& attackerStats = *attacker.getBattleStats();
    BattleStats& defenderStats = *defender.getBattleStats();

    if (skill.hitRate == 100)
        return skill.hitRate;

    float baseAccuracy = (float)(attackerStats.ag + 200) / (defenderStats.ag + 200);
    u32 multipliedAccuracy;
    if (attacker.participantType == ParticipantType::Enemy)
    {
        float shoeMultiplier = (float)(attackerStats.ag + 200) / (defender.shoe->evasion / 2.0f + 200);
        multipliedAccuracy = baseAccuracy * skill.hitRate * shoeMultiplier;
    }
    else
    {
        multipliedAccuracy = (u32)(baseAccuracy * skill.hitRate);
    }

    return std::clamp(multipliedAccuracy, (u32)50, (u32)99);
}

u32 BattleCalcs::healing(BattleParticipant& user, Skill& skill)
{
    float teamMultiplier;
    BattleStats* battleStats;
    if (user.participantType == ParticipantType::Party || user.participantType == ParticipantType::Player)
    {
        teamMultiplier = 1.0f;
        PartyMember& partyMember = static_cast<PartyMember&>(user);
        battleStats = &partyMember.curPersona->battleStats;
    }
    else
    {
        teamMultiplier = 0.6f;
        Enemy& enemy = static_cast<Enemy&>(user);
        battleStats = &enemy.battleStats;
    }

    u32 magicBoost = BattleCalcs::getMagicBoostHeal(battleStats->ma);
    u32 base = (u32)floor((skill.movePower + magicBoost) * teamMultiplier);
    u32 range = 95 + (u32)(rand() % 11);
    return (u32)floor(base * range / 100.0f);
}

/* TODO: on hold
u32 CalcHpDif::calculateDamageAllOutAttack(BattleStats* attackerStats,
                                           BattleStats* defenderStats,
                                           u32* attackerLevel,
                                           u32* defenderLevel,
                                           u32* participantCount)
{
    damageSetup(attackerStats, defenderStats, attackerLevel, defenderLevel, skill);
    //TODO: use weapon damage / 2
    // TODO: hopefully correct, should be looked at by someone that knows some  math
    float base = trunc(sqrt((float)(skill->movePower * 15 * Atk) / defenderStats->en) * 1.6f *
                       (levelDifference * levelDifference) * affinityMtp * *participantCount);
    float range = 95 + (u32)(rand() % 11);
    return (u32)trunc(base * range / 100.0f);
}

*/

void BattleCalcs::damageSetup(
    BattleStats& attackerStats, BattleStats& defenderStats, u32& attackerLevel, u32& defenderLevel, Skill& skill)
{
    if (skill.skillRace == SkillRace::phys)
        Atk = attackerStats.st;
    else
        Atk = attackerStats.ma;

    // TODO: std clamp
    diff = attackerLevel - defenderLevel;
    if (diff < -13)
        diff = -13;
    else if (diff > 10)
        diff = 10;

    levelDifference = levelMultipliers[diff + 13]; // offset so -13 is 0

    u32 affinity = defenderStats.affinities[(u32)skill.element];

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

const float BattleCalcs::levelMultipliers[24] = {0.5f,  0.51f, 0.53f, 0.59f, 0.66f, 0.75f, 0.84f, 0.91f,
                                                 0.97f, 0.99f, 1.0f,  1.0f,  1.0f,  1.0f,  1.01f, 1.03f,
                                                 1.09f, 1.16f, 1.25f, 1.34f, 1.41f, 1.47f, 1.49f, 1.5f};

const float BattleCalcs::magicBoostTableHeal[20] = {
    0,   // 1-5
    6,   // 6-10
    12,  // 11-15
    17,  // 16-20
    22,  // 21-25
    27,  // 26-30
    34,  // 31-35
    44,  // 36-40
    54,  // 41-45
    65,  // 46-50
    75,  // 51-55
    85,  // 56-60
    93,  // 61-65
    100, // 66-70
    105, // 71-75
    110, // 76-80
    115, // 81-85
    120, // 86-90
    125, // 91-95
    130  // 96-99
};

u32 BattleCalcs::getMagicBoostHeal(u32& magic)
{
    u32 index = (magic - 1) / 5;
    index = std::clamp(index, (u32)0, (u32)19);
    return magicBoostTableHeal[index];
}
