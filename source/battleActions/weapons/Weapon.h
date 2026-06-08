#pragma once
#include "../WeaponType.h"
#include <nds.h>

// TODO: we need some kind of selection and matching weaponType validation outside of battle
// in the future
struct Weapon
{
    u32 weaponPower;
    WeaponType hitRate;
    // TODO: add stat boosts / other shenanigans later
};
