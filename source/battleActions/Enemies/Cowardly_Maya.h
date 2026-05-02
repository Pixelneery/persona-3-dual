#pragma once
#include "Enemy.h"
#include "../Skills/Attacking.h"
#include "../Skills/Slash_Attack.h"

struct Cowardly_Maya : Enemy
{
    Slash_Attack slash_Attack;
    Attacking *myAttacking[1];
    Cowardly_Maya() {
        myAttacking[0] = &slash_Attack;

        name = "Cowardly Maya";
        hp = 67;
        sp = 15;
        lv = 2;
        st = 2;
        ma = 4;
        en = 3;
        ag = 2;
        lu = 2;
        attacking = myAttacking;
        attackCount = 1;
    }
};
