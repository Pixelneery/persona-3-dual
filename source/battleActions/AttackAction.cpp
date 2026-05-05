#include "AttackAction.h"
#include <stdio.h>
#include <string.h>

void AttackAction::execute()
{
    inProgress = true;
}

bool AttackAction::update()
{
    u32 keys = keysDown();
    u32 enemyCount = enemies->size();

    updateIndex.update(keys, targetIndex, enemyCount);

    if (keys & KEY_LEFT)
    {
        iprintf("CurTar: ");
        iprintf(enemies->at(targetIndex)->name.c_str());
        iprintf("\n");
    }
    else if (keys & KEY_RIGHT)
    {
        iprintf("CurTar: ");
        iprintf(enemies->at(targetIndex)->name.c_str());
        iprintf("\n");
    }

    if (keys & KEY_A)
    {
        iprintf("AttackSkill: ");
        iprintf(enemies->at(targetIndex)->name.c_str());
        enemies->at(targetIndex)->hp -= player->baseAttackAction->calculateDamage(&player->ma, &player->st, &enemies->at(targetIndex)->en, &player->lv, &enemies->at(targetIndex)->lv);
        iprintf("\n");

        char str[50];
        std::sprintf(str, "remaing Enemy hp: %d \n", enemies->at(targetIndex)->hp);
        iprintf(str);
        iprintf("\n");

        if (enemies->at(targetIndex)->hp <= 0)
        {
            enemies->erase(enemies->begin() + targetIndex);

            if (enemies->empty())
            {
                iprintf("battle over, you won");
                iprintf("\n");
            }
        }

        targetIndex = 0;
        inProgress = false;
        return true;
    }

    if (keys & KEY_B)
    {
        inProgress = false;
    }

    return false;
}