#pragma once
#include <nds.h>
#include "SkillTarget.h"
#include "SkillType.h"
#include "SkillRace.h"

struct Skill
{
    std::string name;
    s32 cost;
    u32 element;
    u32 hitRate;
    u32 movePower;
    SkillType skillType;
    SkillRace skillRace;
    SkillTarget skillTarget;
};
