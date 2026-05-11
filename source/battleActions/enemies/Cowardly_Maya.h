#pragma once
#include "Enemy.h"
#include "../skills/AttackSkill.h"
#include "../skills/Slash_Attack.h"

struct Cowardly_Maya : Enemy
{
    Slash_Attack slash_Attack;
    AttackSkill *myAttackSkill[1];
    Cowardly_Maya()
    {
        myAttackSkill[0] = &slash_Attack;

        name = "Cowardly Maya";
        hp = 83;
        sp = 20;
        lv = 2;
        st = 2;
        ma = 3;
        en = 3;
        ag = 2;
        lu = 2;
        attackSkill = myAttackSkill;
        attackCount = 1;

        affinities[Element::Slash] = Weak;
        affinities[Element::Strike] = Neutral;
        affinities[Element::Pierce] = Neutral;
        affinities[Element::Fire] = Weak;
        affinities[Element::Ice] = Neutral;
        affinities[Element::Electricity] = Neutral;
        affinities[Element::Wind] = Neutral;
        affinities[Element::Light] = Neutral;
        affinities[Element::Dark] = Neutral;
    }
};
