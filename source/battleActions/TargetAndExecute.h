#pragma once
#include <nds.h>
#include <stdio.h>
#include <vector>
#include "skills/Skill.h"
#include "skills/AttackSkill.h"
#include "skills/HealSkill.h"
#include "BattleParticipant.h"
#include "party/PartyMember.h"
#include "DeductAttackCost.h"

struct TargetAndExecute
{
    std::vector<BattleParticipant *> *targets;
    u32 *targetIndex;

    TargetAndExecute(u32 *iTargetIndex) : targetIndex(iTargetIndex) {}

    bool update(u32 *keys, Skill *skill, PartyMember *user, std::vector<BattleParticipant *> *targets);
};
