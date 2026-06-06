#pragma once
#include "PersonaBase.h"

#include "../skills/Power_Slash.h"
#include "../skills/Skill.h"

struct Hermes : PersonaBase
{
    Power_Slash power_Slash;
    Skill* mySkills[1];

    Hermes()
    {
        name = "Hermes";
        lv = 1;
        battleStats.st = 3;
        battleStats.ma = 1;
        battleStats.en = 2;
        battleStats.ag = 3;
        battleStats.lu = 1;

        mySkills[0] = &power_Slash;
        skillCount = 1;
        skills = mySkills;

        battleStats.affinities[(u32)Element::Slash] = BattleStats::Neutral;
        battleStats.affinities[(u32)Element::Strike] = BattleStats::Neutral;
        battleStats.affinities[(u32)Element::Pierce] = BattleStats::Neutral;
        battleStats.affinities[(u32)Element::Fire] = BattleStats::Resist;
        battleStats.affinities[(u32)Element::Ice] = BattleStats::Neutral;
        battleStats.affinities[(u32)Element::Electricity] = BattleStats::Neutral;
        battleStats.affinities[(u32)Element::Wind] = BattleStats::Weak;
        battleStats.affinities[(u32)Element::Light] = BattleStats::Neutral;
        battleStats.affinities[(u32)Element::Dark] = BattleStats::Neutral;
        // add arcana
    }
};
