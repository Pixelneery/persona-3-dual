#pragma once
#include "../skills/Bufu.h"
#include "../skills/Skill.h"
#include "../skills/Strike_Attack.h"
#include "Enemy.h"

struct Cowardly_Maya : Enemy
{
    Strike_Attack strike_Attack;
    Bufu bufu;

    Skill* mySkill[1];
    Cowardly_Maya()
    {
        baseAttackAction = &strike_Attack;
        mySkill[0] = &bufu;

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
        skill = mySkill;
        skillCount = 1;

        battleStats.affinities[(u32)Element::Slash] = BattleStats::Weak;
        battleStats.affinities[(u32)Element::Strike] = BattleStats::Neutral;
        battleStats.affinities[(u32)Element::Pierce] = BattleStats::Neutral;
        battleStats.affinities[(u32)Element::Fire] = BattleStats::Weak;
        battleStats.affinities[(u32)Element::Ice] = BattleStats::Neutral;
        battleStats.affinities[(u32)Element::Electricity] = BattleStats::Neutral;
        battleStats.affinities[(u32)Element::Wind] = BattleStats::Neutral;
        battleStats.affinities[(u32)Element::Light] = BattleStats::Neutral;
        battleStats.affinities[(u32)Element::Dark] = BattleStats::Neutral;
    }
};
