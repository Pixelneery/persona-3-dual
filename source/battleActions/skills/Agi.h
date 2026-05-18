#pragma once
#include "AttackSkill.h"

struct Agi : AttackSkill
{
    Agi()
    {
        moveDamage = 40;
        race = mag;
        element = Fire;
        cost = 3;
        name = "Agi";
        hitRate = 99;
    }
};
