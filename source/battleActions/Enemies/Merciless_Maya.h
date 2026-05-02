#pragma once
#include "Enemy.h"
#include "../Skills/Attacking.h"
#include "../Skills/Slash_Attack.h"
#include "../Skills/Agi.h"

struct Merciless_Maya : Enemy
{
    Slash_Attack slash_Attack;
    Agi agi;
    Attacking *myAttacking[2];
    
    Merciless_Maya() {
        myAttacking[0] = &slash_Attack;
        myAttacking[1] = &agi;

        name = "Merciless Maya";
        hp = 50;
        sp = 21;
        lv = 3;
        st = 4;
        ma = 4;
        en = 3;
        ag = 3;
        lu = 2;
        attacking = myAttacking;
        attackCount = 2;
    }
};
