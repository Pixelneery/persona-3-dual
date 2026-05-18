#pragma once
#include "AttackSkill.h"

struct Bufu : AttackSkill
{
    Bufu()
    {
        moveDamage = 40;
        race = mag;
        element = Ice;
        cost = 3;
        name = "Bufu";
        hitRate = 99;
    }
};
