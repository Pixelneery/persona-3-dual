#pragma once
#include "AttackSkill.h"

struct Pierce_Attack : AttackSkill
{
    Pierce_Attack()
    {
        // TODO: moveDamage should be correct, not confirmed tough yet
        moveDamage = 10;
        race = phys;
        element = Pierce;
        cost = 0;
        name = "Pierce_Attack";
        hitRate = 90;
    }
};
