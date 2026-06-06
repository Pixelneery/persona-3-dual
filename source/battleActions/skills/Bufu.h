#pragma once
#include "AttackSkill.h"

struct Bufu : AttackSkill
{
    Bufu()
    {
        movePower = 40;
        element = Ice;
        cost = 3;
        name = "Bufu";
        hitRate = 99;
        skillRace = SkillRace::mag;
        skillTarget = SkillTarget::OppositionTeam;
        skillType = SkillType::Attack;
    }
};
