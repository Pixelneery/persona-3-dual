#pragma once
#include "../BattleParticipant.h"
#include "../enemies/Enemy.h"
#include "../party/PartyMember.h"
#include "Skill.h"
#include <nds.h>

struct HealSkill : Skill
{
    HealSkill()
    {
        element = Heal;
        skillType = SkillType::Heal;
    }

    u32 calculateHealing(BattleParticipant& user);

  private:
    static const float magicBoostTable[20];
    u32 getMagicBoost(u32& magic);
};
