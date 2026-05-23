#pragma once
#include "AttackSkill.h"

struct Slash_Attack : AttackSkill
{
    Slash_Attack()
    {
        // TODO: movePower should be correct, not confirmed tough yet
        movePower = 10;
        element = Slash;
        cost = 0;
        name = "Slash_Attack";
        hitRate = 90;
        skillRace = SkillRace::phys;
        skillTarget = SkillTarget::OppositionTeam;
    }
};
