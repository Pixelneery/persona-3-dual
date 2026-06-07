#pragma once
#include "Skill.h"

struct Agi : Skill
{
    Agi()
    {
        movePower = 40;
        element = Element::Fire;
        cost = 3;
        name = "Agi";
        hitRate = 99;
        skillRace = SkillRace::mag;
        skillTarget = SkillTarget::OppositionTeam;
        skillType = SkillType::Attack;
    }
};
