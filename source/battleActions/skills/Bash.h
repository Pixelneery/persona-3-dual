#pragma once
#include "AttackSkill.h"

struct Bash : AttackSkill
{
    Bash()
    {
        moveDamage = 30;
        race = phys;
        // TODO: should be 10% of hp, add percantage support later
        cost = 10;
        name = "Bash";
    }
};
