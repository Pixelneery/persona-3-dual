#pragma once
#include <nds.h>
#include "skills/AttackSkill.h"

inline auto DeductAttackCost = [](s32 *stat, s32 cost) -> bool
{
    if (*stat < cost)
        return false;
    *stat -= cost;
    return true;
};
