#pragma once 
#include <nds.h>
#include <string>
#include "../Skills/Attacking.h"

/*
St	Represents strength and physical damage.
Ma	Represents magic and magical damage.
En	Represents endurance, which determines your defense and how much damage you can take.
Ag	Represents agility, which determines your place in the turn order.
Lu	Represents luck, which is taken into account when using certain skills involving status afflictions or insta-death abilities.*/

struct Player
{
    std::string name;
    u32 hp;
    u32 sp;
    u32 lv;
    u32 st;
    u32 ma;
    u32 en;
    u32 ag;
    u32 lu;
    Attacking *baseAttack;
    Attacking **attacking;
    u32 attackCount;

    bool guarding = false;
    //add resitances and arcana in the future

    Player() {}
    ~Player() {};
};

