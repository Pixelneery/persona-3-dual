#pragma once
#include "Armour.h"

struct RashGuard : Armour
{
    RashGuard()
    {
        defense = 46;
        armourType = ArmourType::Unisex;
    }
};
