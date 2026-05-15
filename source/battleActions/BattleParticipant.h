#pragma once
#include <nds.h>
#include <string>
#include "skills/AttackSkill.h"

struct BattleParticipant
{
    std::string name;
    s32 hp;
    s32 sp;
    u32 lv;
    AttackSkill *baseAttackAction;

    bool oneMore = false;
    // knocked will be needed in the future, no use yet
    bool knockedDown = false;
};
