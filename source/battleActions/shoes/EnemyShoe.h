#pragma once
#include "Shoe.h"

struct EnemyShoe : Shoe
{
    EnemyShoe()
    {
        evasion = 10;
        armourType = ArmourType::Enemy;
    }
};
