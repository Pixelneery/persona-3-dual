#pragma once
#include <nds.h>
#include <cmath>
#include <stdlib.h>
#include <stdio.h>
#include <string>
/*
Alleged p3f damage formula:
The square root of ( (Persona Strength or Magic / Enemy Endurance ) * ( Player Level / Enemy Level ) * Move Damage Value ) * 7.4
//Important: The Persona level does not matter for this calculation, the player level does

//TODO: implement randomness
The above formula disregards a random 10% swing in either direction, for example a "100 damage" calculation can be anywhere between 90 and 110 due to RNG.
*/

struct AttackSkill
{
    float moveDamage;
    enum Race
    {
        phys,
        mag
    };
    u32 cost;
    Race race;
    std::string name;
    // bad solution to figure out if magic or physical
    float calculateDamage(u32 *attackerMag, u32 *attackerPhys, u32 *defenderEn, u32 *attackerLevel, u32 *defenderLevel)
    {
        u32 Atk;
        if (race == phys)
            Atk = *attackerMag;
        else
            Atk = *attackerPhys;

        float result = sqrt(
                           ((float)Atk / *defenderEn) *
                           ((float)*attackerLevel / *defenderLevel) *
                           moveDamage) *
                       7.4f;
        return result;
    }
    virtual ~AttackSkill() = default;
};
