#pragma once
#include "AttackSkill.h"

struct Magaru : AttackSkill
{
    Magaru()
    {
        movePower = 40;
        element = Wind;
        cost = 8;
        name = "Magaru";
        hitRate = 95;
        skillRace = SkillRace::mag;
        skillTarget = SkillTarget::OppositionTeam;
        skillType = SkillType::MultiAttack;
    }
};
