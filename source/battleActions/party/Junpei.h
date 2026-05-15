#pragma once
#include "PartyMember.h"
#include "../armours/RashGuard.h"
#include "../personas/Hermes.h"
#include "../skills/Slash_Attack.h"

struct Junpei : PartyMember
{
    RashGuard rashGuard;
    Slash_Attack slash_Attack;
    Hermes hermes;

    Junpei()
    {
        name = "Junpei";
        hp = 70;
        sp = 36;
        lv = 1;

        armourType = ArmourType::Male;
        armour = rashGuard;

        baseAttackAction = &slash_Attack;
        persona = &hermes;
    }
};
