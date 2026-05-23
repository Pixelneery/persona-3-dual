#pragma once
#include "AttackSkill.h"

struct Power_Slash : AttackSkill
{
    Power_Slash()
    {
        // TODO: make cost %S
        movePower = 50;
        element = Slash;
        cost = 7;
        name = "Power_Slash";
        hitRate = 92;
        skillRace = SkillRace::phys;
        skillTarget = SkillTarget::OppositionTeam;
    }
};
