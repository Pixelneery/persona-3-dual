#pragma once
#include "AttackSkill.h"

struct Agi : AttackSkill
{
    Agi()
    {
        moveDamage = 40;
        race = mag;
        cost = 3;
        name = "Agi";
    }
};
