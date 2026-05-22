#pragma once
#include "AttackSkill.h"

struct Agi : AttackSkill
{
    Agi()
    {
        movePower = 40;
        element = Fire;
        cost = 3;
        name = "Agi";
        hitRate = 99;
        skillRace = SkillRace::mag;
        skillTarget = SkillTarget::OppositionTeam;
        skillType = SkillType::Attack;
    }
};
