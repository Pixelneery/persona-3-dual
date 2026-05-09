#pragma once
#include "AttackSkill.h"

struct Bufu : AttackSkill
{
    Bufu()
    {
        moveDamage = 40;
        race = mag;
        cost = 3;
        name = "Bufu";
    }
};
