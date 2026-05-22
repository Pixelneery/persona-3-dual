#pragma once
#include "AttackSkill.h"

struct Dia : AttackSkill
{
    Dia()
    {
        movePower = 50;
        element = Heal;
        cost = 3;
        name = "Dia";
        hitRate = 100;
        skillRace = SkillRace::mag;
        skillTarget = SkillTarget::OwnTeam;
        skillType = SkillType::Heal;
    }
};
