#pragma once
#include "AttackSkill.h"

struct Pierce_Attack : AttackSkill
{
    Pierce_Attack()
    {
        // TODO: movePower should be correct, not confirmed tough yet
        movePower = 10;
        element = Pierce;
        cost = 0;
        name = "Pierce_Attack";
        hitRate = 90;
        skillRace = SkillRace::phys;
        skillTarget = SkillTarget::OppositionTeam;
        skillType = SkillType::Attack;
    }
};
