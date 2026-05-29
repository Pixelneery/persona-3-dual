#pragma once
#include "PersonaBase.h"

#include "../skills/AttackSkill.h"
#include "../skills/Dia.h"
#include "../skills/Garu.h"

struct Io : PersonaBase
{
    Garu garu;
    Dia dia;
    Skill* mySkills[2];

    Io()
    {
        name = "Io";
        lv = 1;
        battleStats.st = 2;
        battleStats.ma = 3;
        battleStats.en = 1;
        battleStats.ag = 2;
        battleStats.lu = 2;

        mySkills[0] = &garu;
        mySkills[1] = &dia;
        attackCount = 2;
        skills = mySkills;

        battleStats.affinities[Element::Slash] = BattleStats::Neutral;
        battleStats.affinities[Element::Strike] = BattleStats::Neutral;
        battleStats.affinities[Element::Pierce] = BattleStats::Neutral;
        battleStats.affinities[Element::Fire] = BattleStats::Neutral;
        battleStats.affinities[Element::Ice] = BattleStats::Neutral;
        battleStats.affinities[Element::Electricity] = BattleStats::Weak;
        battleStats.affinities[Element::Wind] = BattleStats::Resist;
        battleStats.affinities[Element::Light] = BattleStats::Neutral;
        battleStats.affinities[Element::Dark] = BattleStats::Neutral;
        // add arcana
    }
};
