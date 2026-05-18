#pragma once
#include "AttackSkill.h"

struct Power_Slash : AttackSkill
{
    Power_Slash()
    {
        // TODO: make cost %S
        moveDamage = 50;
        race = phys;
        element = Slash;
        cost = 7;
        name = "Power_Slash";
        hitRate = 92;
    }
};
