#pragma once
#include "Shoe.h"

struct RubberSole : Shoe
{
    RubberSole()
    {
        evasion = 19;
        armourType = ArmourType::Unisex;
    }
};
