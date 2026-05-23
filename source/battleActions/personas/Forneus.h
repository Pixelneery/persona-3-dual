#pragma once
#include "PersonaBase.h"

#include "../skills/AttackSkill.h"
#include "../skills/Bash.h"
#include "../skills/Bufu.h"

struct Forneus : PersonaBase
{
    Bash bash;
    Bufu bufu;
    Skill *mySkills[2];

    Forneus()
    {
        name = "Forneus";
        lv = 7;
        battleStats.st = 5;
        battleStats.ma = 6;
        battleStats.en = 7;
        battleStats.ag = 6;
        battleStats.lu = 4;

        mySkills[0] = &bash;
        mySkills[1] = &bufu;
        attackCount = 2;
        skills = mySkills;

        battleStats.affinities[Element::Slash] = BattleStats::Neutral;
        battleStats.affinities[Element::Strike] = BattleStats::Neutral;
        battleStats.affinities[Element::Pierce] = BattleStats::Neutral;
        battleStats.affinities[Element::Fire] = BattleStats::Weak;
        battleStats.affinities[Element::Ice] = BattleStats::Resist;
        battleStats.affinities[Element::Electricity] = BattleStats::Neutral;
        battleStats.affinities[Element::Wind] = BattleStats::Neutral;
        battleStats.affinities[Element::Light] = BattleStats::Neutral;
        battleStats.affinities[Element::Dark] = BattleStats::Neutral;
        // add arcana
    }
};
