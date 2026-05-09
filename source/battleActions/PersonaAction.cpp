#include "PersonaAction.h"
#include <stdio.h>

void PersonaAction::execute()
{
    inProgress = true;
    targetIndex = 0;
    menuState = SelectSkill;
}

bool PersonaAction::update(u32 *keys)
{
    if (menuState == SelectSkill)
    {
        u32 skillCount = player->attackCount;

        updateIndex.update(*keys, targetIndex, skillCount);

        if (*keys & KEY_LEFT)
        {
            iprintf("Cur: ");
            iprintf(player->attackSkill[targetIndex]->name.c_str());
            iprintf("\n");
        }
        else if (*keys & KEY_RIGHT)
        {
            iprintf("Cur: ");
            iprintf(player->attackSkill[targetIndex]->name.c_str());
            iprintf("\n");
        }

        if (*keys & KEY_A)
        {
            if (player->attackSkill[targetIndex]->race == AttackSkill::mag)
            {
                if (!DeductAttackCost(&player->sp, player->attackSkill[targetIndex]->cost, "not enough SP\n"))
                    return false;
            }
            else if (player->attackSkill[targetIndex]->race == AttackSkill::phys)
            {
                if (!DeductAttackCost(&player->hp, player->attackSkill[targetIndex]->cost, "not enough HP\n"))
                    return false;
            }

            iprintf("Sel: ");
            selectedSkill = player->attackSkill[targetIndex];
            iprintf(selectedSkill->name.c_str());
            iprintf("\n");

            targetIndex = 0;
            menuState = SelectTarget;
        }
    }
    else if (menuState == SelectTarget)
    {
        u32 enemyCount = enemies->size();

        updateIndex.update(*keys, targetIndex, enemyCount);

        bool madeAction = targetAndAttackActionEnemy->update(keys, selectedSkill);
        if (madeAction)
        {
            targetIndex = 0;
            inProgress = false;
            return true;
        }
    }

    if (*keys & KEY_B)
    {
        if (menuState == SelectSkill)
        {
            inProgress = false;
        }
        else if (menuState == SelectTarget)
        {
            targetIndex = 0;
            menuState = SelectSkill;
        }
    }

    return false;
}
