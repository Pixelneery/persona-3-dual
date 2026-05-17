#pragma once
#include "Enemy.h"
#include "../skills/AttackSkill.h"
#include "../skills/Strike_Attack.h"
#include "../skills/Agi.h"

struct Merciless_Maya : Enemy
{
    Strike_Attack strike_Attack;
    Agi agi;
    AttackSkill *myAttackSkill[1];

    Merciless_Maya()
    {
        baseAttackAction = &strike_Attack;
        myAttackSkill[0] = &agi;

        name = "Merciless Maya";
        hp = 83;
        sp = 15;
        lv = 2;
        battleStats.st = 2;
        battleStats.ma = 4;
        battleStats.en = 3;
        battleStats.ag = 2;
        battleStats.lu = 2;
        attackSkill = myAttackSkill;
        attackCount = 1;

        battleStats.affinities[Element::Slash] = BattleStats::Weak;
        battleStats.affinities[Element::Strike] = BattleStats::Neutral;
        battleStats.affinities[Element::Pierce] = BattleStats::Neutral;
        battleStats.affinities[Element::Fire] = BattleStats::Resist;
        battleStats.affinities[Element::Ice] = BattleStats::Weak;
        battleStats.affinities[Element::Electricity] = BattleStats::Null;
        battleStats.affinities[Element::Wind] = BattleStats::Neutral;
        battleStats.affinities[Element::Light] = BattleStats::Neutral;
        battleStats.affinities[Element::Dark] = BattleStats::Neutral;
    }
};
