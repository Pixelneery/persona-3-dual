#pragma once
#include <nds.h>
#include <string>
#include "../skills/AttackSkill.h"
#include "../personas/PersonaBase.h"
#include "../armourType.h"
#include "../armours/Armour.h"

/*
St	Represents strength and physical damage.
Ma	Represents magic and magical damage.
En	Represents endurance, which determines your defense and how much damage you can take.
Ag	Represents agility, which determines your place in the turn order.
Lu	Represents luck, which is taken into account when using certain skills involving status afflictions or insta-death abilities.*/

struct Player
{
    std::string name;
    s32 hp;
    s32 sp;
    u32 lv;
    ArmourType armourType;
    Armour armour;
    AttackSkill *baseAttackAction;
    PersonaBase **personas;
    PersonaBase *curPersona;
    u32 personaCount;

    bool guarding = false;
    bool oneMore = false;
    bool knockedDown = false;

    Player() {}
    ~Player() {};
};
