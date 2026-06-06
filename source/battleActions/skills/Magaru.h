#pragma once
#include "Skill.h"

struct Magaru : Skill
{
    Magaru()
    {
        movePower = 40;
        element = Element::Wind;
        cost = 8;
        name = "Magaru";
        hitRate = 95;
        skillRace = SkillRace::mag;
        skillTarget = SkillTarget::OppositionTeam;
        skillType = SkillType::MultiAttack;
    }
};
