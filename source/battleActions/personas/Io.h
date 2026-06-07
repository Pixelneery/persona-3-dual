#pragma once
#include "PersonaBase.h"

#include "../skills/Dia.h"
#include "../skills/Garu.h"
#include "../skills/Magaru.h"
#include "../skills/Skill.h"

struct Io : PersonaBase
{
    Garu garu;
    Dia dia;
    Magaru magaru;
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
        mySkills[2] = &magaru;
        skillCount = 3;
        skills = mySkills;

        battleStats.affinities[(u32)Element::Slash] = BattleStats::Neutral;
        battleStats.affinities[(u32)Element::Strike] = BattleStats::Neutral;
        battleStats.affinities[(u32)Element::Pierce] = BattleStats::Neutral;
        battleStats.affinities[(u32)Element::Fire] = BattleStats::Neutral;
        battleStats.affinities[(u32)Element::Ice] = BattleStats::Neutral;
        battleStats.affinities[(u32)Element::Electricity] = BattleStats::Weak;
        battleStats.affinities[(u32)Element::Wind] = BattleStats::Resist;
        battleStats.affinities[(u32)Element::Light] = BattleStats::Neutral;
        battleStats.affinities[(u32)Element::Dark] = BattleStats::Neutral;
        // add arcana
    }
};
