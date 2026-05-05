#pragma once
#include "Enemy.h"
#include "../skills/AttackSkill.h"
#include "../skills/Slash_Attack.h"

struct Cowardly_Maya : Enemy
{
    Slash_Attack slash_Attack;
    AttackSkill *myAttackSkill[1];
    Cowardly_Maya() {
        myAttackSkill[0] = &slash_Attack;

        name = "Cowardly Maya";
        hp = 67;
        sp = 15;
        lv = 2;
        st = 2;
        ma = 4;
        en = 3;
        ag = 2;
        lu = 2;
        attackSkill = myAttackSkill;
        attackCount = 1;
    }
};
