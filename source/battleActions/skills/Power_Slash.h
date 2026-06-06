#pragma once
#include "Skill.h"

struct Power_Slash : Skill
{
    Power_Slash()
    {
        // TODO: make cost %S
        movePower = 50;
        element = Element::Slash;
        cost = 7;
        name = "Power_Slash";
        hitRate = 92;
        skillRace = SkillRace::phys;
        skillTarget = SkillTarget::OppositionTeam;
        skillType = SkillType::Attack;
    }
};
