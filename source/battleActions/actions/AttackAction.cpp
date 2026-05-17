#include "AttackAction.h"
#include <stdio.h>
#include <string.h>

void AttackAction::execute()
{
    inProgress = true;
    targetIndex = 0;
}

bool AttackAction::update(u32 *keys, PartyMember *user)
{
    updateIndex.update(*keys, targetIndex, enemies->size());

    bool madeAction = targetAndAttackActionEnemy->update(keys, user->baseAttackAction, user);
    if (madeAction)
    {
        targetIndex = 0;
        inProgress = false;
    }

    if (*keys & KEY_B)
    {
        inProgress = false;
    }

    return madeAction;
}
