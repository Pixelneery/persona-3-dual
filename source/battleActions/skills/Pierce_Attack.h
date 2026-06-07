#pragma once
#include "Skill.h"

struct Pierce_Attack : Skill
{
    Pierce_Attack()
    {
        // TODO: movePower should be correct, not confirmed tough yet
        movePower = 10;
        element = Element::Pierce;
        cost = 0;
        name = "Pierce_Attack";
        hitRate = 90;
        skillRace = SkillRace::phys;
        skillTarget = SkillTarget::OppositionTeam;
        skillType = SkillType::RegularAttack;
    }
};
