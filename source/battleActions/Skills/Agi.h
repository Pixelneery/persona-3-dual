#pragma once
#include "Attacking.h"

struct Agi : Attacking 
{
    Agi() {
        moveDamage = 50;
        race = mag;
        cost = 3;
        name = "Agi";
    }
};
