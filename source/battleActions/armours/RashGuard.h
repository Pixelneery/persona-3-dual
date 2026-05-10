#pragma once
#include "Armour.h"

struct PlainShirt : Armour
{
    PlainShirt()
    {
        defense = 46;
        gender = Gender::Unisex;
    }
};
