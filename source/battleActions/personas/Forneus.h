#pragma once
#include "PersonaBase.h"

#include "../skills/AttackSkill.h"
#include "../skills/Bash.h"
#include "../skills/Bufu.h"

struct Forneus : PersonaBase
{
    Bash bash;
    Bufu bufu;
    AttackSkill *myAttcking[2];

    Forneus()
    {
        name = "Forneus";
        lv = 7;
        st = 5;
        ma = 6;
        en = 7;
        ag = 6;
        lu = 4;

        myAttcking[0] = &bash;
        myAttcking[1] = &bufu;
        attackCount = 2;
        attackSkill = myAttcking;

        affinities[Element::Slash] = Neutral;
        affinities[Element::Strike] = Neutral;
        affinities[Element::Pierce] = Neutral;
        affinities[Element::Fire] = Weak;
        affinities[Element::Ice] = Resist;
        affinities[Element::Electricity] = Neutral;
        affinities[Element::Wind] = Neutral;
        affinities[Element::Light] = Neutral;
        affinities[Element::Dark] = Neutral;
        // add arcana
    }
};
