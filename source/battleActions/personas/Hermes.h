#pragma once
#include "PersonaBase.h"

#include "../skills/AttackSkill.h"
#include "../skills/Power_Slash.h"

struct Hermes : PersonaBase
{
    Power_Slash power_Slash;
    AttackSkill *myAttcking[1];

    Hermes()
    {
        name = "Hermes";
        lv = 1;
        st = 3;
        ma = 1;
        en = 2;
        ag = 3;
        lu = 1;

        myAttcking[0] = &power_Slash;
        attackCount = 1;
        attackSkill = myAttcking;

        affinities[Element::Slash] = Neutral;
        affinities[Element::Strike] = Neutral;
        affinities[Element::Pierce] = Neutral;
        affinities[Element::Fire] = Resist;
        affinities[Element::Ice] = Neutral;
        affinities[Element::Electricity] = Neutral;
        affinities[Element::Wind] = Weak;
        affinities[Element::Light] = Neutral;
        affinities[Element::Dark] = Neutral;
        // add arcana
    }
};
