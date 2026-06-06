#pragma once
#include "Skill.h"

struct Bufu : Skill
{
    Bufu()
    {
        movePower = 40;
        element = Element::Ice;
        cost = 3;
        name = "Bufu";
        hitRate = 99;
        skillRace = SkillRace::mag;
        skillTarget = SkillTarget::OppositionTeam;
        skillType = SkillType::Attack;
    }
};
