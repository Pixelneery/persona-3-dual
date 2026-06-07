#pragma once
#include "Skill.h"

struct Strike_Attack : Skill
{
    Strike_Attack()
    {
        // TODO: movePower should be correct, not confirmed tough yet
        movePower = 10;
        element = Element::Strike;
        cost = 0;
        name = "Strike_Attack";
        hitRate = 90;
        skillRace = SkillRace::phys;
        skillTarget = SkillTarget::OppositionTeam;
        skillType = SkillType::RegularAttack;
    }
};
