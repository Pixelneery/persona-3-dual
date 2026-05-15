#pragma once
#include <nds.h>
#include <string>
#include "../skills/AttackSkill.h"
#include "../personas/PersonaBase.h"
#include "../ArmourType.h"
#include "../armours/Armour.h"
#include "../BattleParticipant.h"

/*
St	Represents strength and physical damage.
Ma	Represents magic and magical damage.
En	Represents endurance, which determines your defense and how much damage you can take.
Ag	Represents agility, which determines your place in the turn order.
Lu	Represents luck, which is taken into account when using certain skills involving status afflictions or insta-death abilities.*/

struct Player : BattleParticipant
{
    ArmourType armourType;
    Armour armour;
    PersonaBase **personas;
    PersonaBase *curPersona;
    u32 personaCount;

    bool guarding = false;

    Player() {}
    ~Player() {};
};
