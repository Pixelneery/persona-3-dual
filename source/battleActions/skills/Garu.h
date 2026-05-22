#pragma once
#include "AttackSkill.h"

struct Garu : AttackSkill
{
    Garu()
    {
        movePower = 40;
        element = Wind;
        cost = 3;
        name = "Garu";
        hitRate = 99;
        skillRace = SkillRace::phys;
        skillTarget = SkillTarget::OppositionTeam;
        skillType = SkillType::Attack;
    }
};
