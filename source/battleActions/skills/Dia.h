#pragma once
#include "HealSkill.h"

struct Dia : HealSkill
{
    Dia()
    {
        movePower = 50;
        cost = 3;
        name = "Dia";
        hitRate = 100;
        skillRace = SkillRace::mag;
        skillTarget = SkillTarget::OwnTeam;
        skillType = SkillType::Heal;
    }
};
