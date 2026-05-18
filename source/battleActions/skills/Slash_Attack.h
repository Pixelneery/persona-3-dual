#pragma once
#include "AttackSkill.h"

struct Slash_Attack : AttackSkill
{
    Slash_Attack()
    {
        // TODO: moveDamage should be correct, not confirmed tough yet
        moveDamage = 10;
        race = phys;
        element = Slash;
        cost = 0;
        name = "Slash_Attack";
        hitRate = 90;
    }
};
