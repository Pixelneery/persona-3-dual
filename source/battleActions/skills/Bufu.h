#pragma once
#include "AttackSkill.h"

struct Bufu : AttackSkill 
{
    Bufu() {
        moveDamage = 45;
        race = mag;
        cost = 3;
        name = "Bufu";
    }
};

