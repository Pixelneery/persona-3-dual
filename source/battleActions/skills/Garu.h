#pragma once
#include "Skill.h"

struct Garu : Skill
{
    Garu()
    {
        movePower = 40;
        element = Element::Wind;
        cost = 3;
        name = "Garu";
        hitRate = 99;
        skillRace = SkillRace::mag;
        skillTarget = SkillTarget::OppositionTeam;
        skillType = SkillType::Attack;
    }
};
