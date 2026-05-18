#pragma once
#include "AttackSkill.h"

struct Strike_Attack : AttackSkill
{
    Strike_Attack()
    {
        // TODO: moveDamage should be correct, not confirmed tough yet
        moveDamage = 10;
        race = phys;
        element = Strike;
        cost = 0;
        name = "Strike_Attack";
        hitRate = 90;
    }
};
