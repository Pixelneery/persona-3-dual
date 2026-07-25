#pragma once
#include "../BattleStats.h"
#include "../ProfileBase.h"
#include "../skills/Skill.h"
#include <nds.h>
#include <string>

struct EnemyProfile : ProfileBase
{
    BattleStats battleStats;
    Skill** skill;
    u32 skillCount;
};
