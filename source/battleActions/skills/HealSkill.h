#pragma once
#include <nds.h>
#include "Skill.h"
#include "../BattleParticipant.h"
#include "../party/PartyMember.h"
#include "../enemies/Enemy.h"

struct HealSkill : Skill
{
    HealSkill()
    {
        element = Heal;
        skillType = SkillType::Heal;
    }

    u32 calculateHealing(BattleParticipant &user);

private:
    static const float magicBoostTable[20];
    u32 getMagicBoost(u32 &magic);
};
