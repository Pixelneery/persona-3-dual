#pragma once
#include "PersonaBase.h"

#include "../skills/Bash.h"
#include "../skills/Bufu.h"
#include "../skills/Skill.h"

struct Forneus : PersonaBase
{
    Bash bash;
    Bufu bufu;
    Skill* mySkills[2];

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
        skillCount = 2;
        skills = mySkills;

        battleStats.affinities[(u32)Element::Slash] = BattleStats::Neutral;
        battleStats.affinities[(u32)Element::Strike] = BattleStats::Neutral;
        battleStats.affinities[(u32)Element::Pierce] = BattleStats::Neutral;
        battleStats.affinities[(u32)Element::Fire] = BattleStats::Weak;
        battleStats.affinities[(u32)Element::Ice] = BattleStats::Resist;
        battleStats.affinities[(u32)Element::Electricity] = BattleStats::Neutral;
        battleStats.affinities[(u32)Element::Wind] = BattleStats::Neutral;
        battleStats.affinities[(u32)Element::Light] = BattleStats::Neutral;
        battleStats.affinities[(u32)Element::Dark] = BattleStats::Neutral;
        // add arcana
    }
};
