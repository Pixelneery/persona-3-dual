#pragma once
#include <nds.h>
#include "../Gender.h"

// TODO: we need some kind of selection and matching gender validation outside of battle
// in the future
struct Armour
{
    u32 defense = 10;
    Gender gender = Gender::Unisex;
    // TODO: add stat boost / other shenanigans later
};
