#pragma once
#include "PersonaBase.h"

#include "../skills/AttackSkill.h"
#include "../skills/Agi.h"
#include "../skills/Bash.h"

struct Orpheus : PersonaBase
{
    Agi agi;
    Bash bash;
    Skill *mySkills[2];

    Orpheus()
    {
        name = "Orpheus";
        lv = 1;
        battleStats.st = 2;
        battleStats.ma = 2;
        battleStats.en = 2;
        battleStats.ag = 2;
        battleStats.lu = 2;

        mySkills[0] = &agi;
        mySkills[1] = &bash;
        attackCount = 2;
        skills = mySkills;

        battleStats.affinities[Element::Slash] = BattleStats::Neutral;
        battleStats.affinities[Element::Strike] = BattleStats::Neutral;
        battleStats.affinities[Element::Pierce] = BattleStats::Neutral;
        battleStats.affinities[Element::Fire] = BattleStats::Neutral;
        battleStats.affinities[Element::Ice] = BattleStats::Neutral;
        battleStats.affinities[Element::Electricity] = BattleStats::Weak;
        battleStats.affinities[Element::Wind] = BattleStats::Neutral;
        battleStats.affinities[Element::Light] = BattleStats::Neutral;
        battleStats.affinities[Element::Dark] = BattleStats::Weak;
        // add arcana
    }
};
