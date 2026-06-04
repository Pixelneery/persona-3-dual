#pragma once
#include "../skills/AttackSkill.h"
#include "../skills/Bufu.h"
#include "../skills/Strike_Attack.h"
#include "Enemy.h"

struct Cowardly_Maya : Enemy
{
    Strike_Attack strike_Attack;
    Bufu bufu;

    AttackSkill* myAttackSkill[1];
    Cowardly_Maya()
    {
        baseAttackAction = &strike_Attack;
        myAttackSkill[0] = &bufu;

        name = "Cowardly Maya";

        maxHp = 83;
        hp = 83;
        maxSp = 20;
        sp = 20;
        lv = 2;
        battleStats.st = 2;
        battleStats.ma = 3;
        battleStats.en = 3;
        battleStats.ag = 2;
        battleStats.lu = 2;
        attackSkill = myAttackSkill;
        skillCount = 1;

        battleStats.affinities[Element::Slash] = BattleStats::Weak;
        battleStats.affinities[Element::Strike] = BattleStats::Neutral;
        battleStats.affinities[Element::Pierce] = BattleStats::Neutral;
        battleStats.affinities[Element::Fire] = BattleStats::Weak;
        battleStats.affinities[Element::Ice] = BattleStats::Neutral;
        battleStats.affinities[Element::Electricity] = BattleStats::Neutral;
        battleStats.affinities[Element::Wind] = BattleStats::Neutral;
        battleStats.affinities[Element::Light] = BattleStats::Neutral;
        battleStats.affinities[Element::Dark] = BattleStats::Neutral;
    }
};
