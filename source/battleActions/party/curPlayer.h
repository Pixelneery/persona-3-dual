#pragma once
#include <nds.h>
#include <string>
#include "../skills/AttackSkill.h"
#include "../skills/Slash_Attack.h"
#include "../personas/PersonaBase.h"
#include "../personas/Orpheus.h"
#include "../personas/Forneus.h"
#include "../armours/PlainShirt.h"
// #include "../armours/RashGuard.h"
#include "Player.h"

/*
St	Represents strength and physical damage.
Ma	Represents magic and magical damage.
En	Represents endurance, which determines your defense and how much damage you can take.
Ag	Represents agility, which determines your place in the turn order.
Lu	Represents luck, which is taken into account when using certain skills involving status afflictions or insta-death abilities.*/

// TODO: since these will be scalled with level etc this abosultey needs to be in some gloabl state
struct curPlayer : Player
{
    Slash_Attack slash_Attack;

    Orpheus orpheus;
    Forneus forneus;
    PersonaBase *myPersonas[2];

    PlainShirt plainShirt;

    curPlayer()
    {
        name = "Makoto";
        hp = 72;
        sp = 50;
        lv = 2;

        armourType = ArmourType::Male;
        armour = plainShirt;

        baseAttackAction = &slash_Attack;
        myPersonas[0] = &orpheus;
        myPersonas[1] = &forneus;
        personaCount = 2;
        personas = myPersonas;
        curPersona = myPersonas[0];
    }
};
