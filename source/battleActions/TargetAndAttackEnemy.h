#pragma once
#include <nds.h>
#include <stdio.h>
#include <vector>
#include "skills/AttackSkill.h"
#include "BattleParticipant.h"
#include "party/PartyMember.h"
#include "DeductAttackCost.h"

struct TargetAndAttackActionEnemy
{
    std::vector<BattleParticipant *> *targets;
    u32 *targetIndex;

    TargetAndAttackActionEnemy(std::vector<BattleParticipant *> *iTargets, u32 *iTargetIndex) : targets(iTargets), targetIndex(iTargetIndex) {}

    bool update(u32 *keys, AttackSkill *attack, PartyMember *user);
};
