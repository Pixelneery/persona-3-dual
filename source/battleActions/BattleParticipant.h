#pragma once
#include "BattleStats.h"
#include "ParticipantType.h"
#include "armours/Armour.h"
#include "shoes/Shoe.h"
#include "skills/Skill.h"
#include <nds.h>
#include <vector>

struct BattleParticipant
{
    std::string name;
    s32 maxHp;
    s32 hp;
    s32 maxSp;
    s32 sp;
    u32 lv;
    // TODO: dont think so that all bosses have a basattack, possibly move in the future
    Skill* baseAttackAction;
    ParticipantType participantType;

    //Enemies have default values
    Armour* armour;
    Shoe* shoe;

    float currentTurnOrderAgility;
    bool oneMore = false;
    bool knockedDown = false;

    virtual BattleStats* getBattleStats() = 0;
};
