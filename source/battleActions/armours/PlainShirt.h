#pragma once
#include "Armour.h"

struct PlainShirt : Armour
{
    PlainShirt()
    {
        defense = 24;
        armourType = ArmourType::Male;
    }
};
