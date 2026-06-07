#pragma once
#include "Skill.h"

struct Slash_Attack : Skill
{
    Slash_Attack()
    {
        // TODO: movePower should be correct, not confirmed tough yet
        movePower = 10;
        element = Element::Slash;
        cost = 0;
        name = "Slash_Attack";
        hitRate = 90;
        skillRace = SkillRace::phys;
        skillTarget = SkillTarget::OppositionTeam;
        skillType = SkillType::RegularAttack;
    }
};
