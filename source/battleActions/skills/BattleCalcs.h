#pragma once
#include "../BattleParticipant.h"
#include "../BattleStats.h"
#include "../shoes/Shoe.h"
#include "Skill.h"
#include <algorithm>
#include <cmath>
#include <nds.h>
#include <string>

struct BattleCalcs
{
    static u32 attack(BattleParticipant& attacker, BattleParticipant& defender, Skill& skill);
    static u32 hitrate(BattleParticipant& attacker, BattleParticipant& defender, Skill& skill);
    static u32 healing(BattleParticipant& user, Skill& skill);

    static u32 allOutAttack(BattleStats* attackerStats,
                            BattleStats* defenderStats,
                            u32* attackerLevel,
                            u32* defenderLevel,
                            u32* participantCount);

  private:
    //Attack
    static const float levelMultipliers[24];
    static u32 Atk;
    static float affinityMtp;
    static s32 diff;
    static float levelDifference;
    static void damageSetup(
        BattleStats& attackerStats, BattleStats& defenderStats, u32& attackerLevel, u32& defenderLevel, Skill& skill);

    //Heal
    static const float magicBoostTableHeal[20];
    static u32 getMagicBoostHeal(u32& magic);
};
