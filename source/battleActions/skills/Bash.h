#pragma once
#include "Skill.h"

struct Bash : Skill
{
    Bash()
    {
        movePower = 30;
        element = Element::Strike;
        // TODO: should be 10% of hp, add percantage support later
        cost = 10;
        name = "Bash";
        hitRate = 90;
        skillRace = SkillRace::phys;
        skillTarget = SkillTarget::OppositionTeam;
        skillType = SkillType::Attack;
    }
};
