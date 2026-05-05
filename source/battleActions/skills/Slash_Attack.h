#pragma once
#include "AttackSkill.h"

struct Slash_Attack : AttackSkill 
{
    Slash_Attack() {
        //TODO: made up damage, cant find actual value
        moveDamage = 30;
        race = phys;
        cost = 0;
        name = "Slash_Attack";
    }
};
