#pragma once
#include "AttackSkill.h"

struct Strike_Attack : AttackSkill
{
    Strike_Attack()
    {
        // TODO: movePower should be correct, not confirmed tough yet
        movePower = 10;
        element = Strike;
        cost = 0;
        name = "Strike_Attack";
        hitRate = 90;
        skillRace = SkillRace::phys;
        skillTarget = SkillTarget::OppositionTeam;
    }
};
