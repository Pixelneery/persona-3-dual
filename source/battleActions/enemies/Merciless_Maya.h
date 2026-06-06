#pragma once
#include "../skills/Agi.h"
#include "../skills/Skill.h"
#include "../skills/Strike_Attack.h"
#include "Enemy.h"

struct Merciless_Maya : Enemy
{
    Strike_Attack strike_Attack;
    Agi agi;
    Skill* mySkills[1];

    Merciless_Maya()
    {
        baseAttackAction = &strike_Attack;
        mySkills[0] = &agi;

        name = "Merciless Maya";
        maxHp = 83;
        hp = 83;
        maxSp = 15;
        sp = 15;
        lv = 2;
        battleStats.st = 2;
        battleStats.ma = 4;
        battleStats.en = 3;
        battleStats.ag = 2;
        battleStats.lu = 2;
        skill = mySkills;
        skillCount = 1;

        battleStats.affinities[(u32)Element::Slash] = BattleStats::Weak;
        battleStats.affinities[(u32)Element::Strike] = BattleStats::Neutral;
        battleStats.affinities[(u32)Element::Pierce] = BattleStats::Neutral;
        battleStats.affinities[(u32)Element::Fire] = BattleStats::Resist;
        battleStats.affinities[(u32)Element::Ice] = BattleStats::Weak;
        battleStats.affinities[(u32)Element::Electricity] = BattleStats::Null;
        battleStats.affinities[(u32)Element::Wind] = BattleStats::Neutral;
        battleStats.affinities[(u32)Element::Light] = BattleStats::Neutral;
        battleStats.affinities[(u32)Element::Dark] = BattleStats::Neutral;
    }
};
