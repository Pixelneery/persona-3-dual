#pragma once
#include "Armour.h"

struct EnemyArmour : Armour
{
    EnemyArmour()
    {
        defense = 10;
        armourType = ArmourType::Enemy;
    }
};
