#pragma once
#include "Skill.h"

struct Dia : Skill
{
    Dia()
    {
        movePower = 50;
        element = Element::Heal;
        cost = 3;
        name = "Dia";
        hitRate = 100;
        skillRace = SkillRace::mag;
        skillTarget = SkillTarget::OwnTeam;
        skillType = SkillType::Heal;
    }
};
