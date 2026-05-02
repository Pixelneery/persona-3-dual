#pragma once
#include "Attacking.h"

struct Slash_Attack : Attacking 
{
    Slash_Attack() {
        //TODO: made up damage, cant find actual value
        moveDamage = 30;
        race = phys;
        cost = 0;
        name = "Slash_Attack";
    }
};
