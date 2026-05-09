#pragma once
#include "PersonaBase.h"

#include "../skills/AttackSkill.h"
#include "../skills/Bash.h"
#include "../skills/Bufu.h"

struct Forneus : PersonaBase
{
    Bash bash;
    Bufu bufu;
    AttackSkill *myAttcking[2];

    Forneus()
    {
        name = "Forneus";
        lv = 7;
        st = 5;
        ma = 5;
        en = 8;
        ag = 6;
        lu = 4;

        myAttcking[0] = &bash;
        myAttcking[1] = &bufu;
        attackCount = 2;
        attackSkill = myAttcking;

        // add resistances and arcana in the future
    }
};
