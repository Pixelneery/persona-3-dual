#pragma once
#include "PersonaBase.h"

#include "../skills/Agi.h"
#include "../skills/Bash.h"
#include "../skills/Skill.h"

struct Orpheus : PersonaBase
{
    Agi agi;
    Bash bash;
    Skill* mySkills[2];

    Orpheus()
    {
        name = "Orpheus";
        lv = 1;
        battleStats.st = 2;
        battleStats.ma = 2;
        battleStats.en = 2;
        battleStats.ag = 3;
        battleStats.lu = 2;

        mySkills[0] = &agi;
        mySkills[1] = &bash;
        skillCount = 2;
        skills = mySkills;

        battleStats.affinities[(u32)Element::Slash] = BattleStats::Neutral;
        battleStats.affinities[(u32)Element::Strike] = BattleStats::Neutral;
        battleStats.affinities[(u32)Element::Pierce] = BattleStats::Neutral;
        battleStats.affinities[(u32)Element::Fire] = BattleStats::Neutral;
        battleStats.affinities[(u32)Element::Ice] = BattleStats::Neutral;
        battleStats.affinities[(u32)Element::Electricity] = BattleStats::Weak;
        battleStats.affinities[(u32)Element::Wind] = BattleStats::Neutral;
        battleStats.affinities[(u32)Element::Light] = BattleStats::Neutral;
        battleStats.affinities[(u32)Element::Dark] = BattleStats::Weak;
        // add arcana
    }
};
