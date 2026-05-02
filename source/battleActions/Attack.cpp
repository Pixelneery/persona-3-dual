#include "Attack.h"
#include <stdio.h>
#include <string.h>

void Attack::execute()
{
    inProgress = true;

    while (inProgress)
    {
        scanKeys();

        u32 keys = keysDown();
        if (keys & KEY_LEFT)
        {
            if (targetIndex > 0)
            {
                targetIndex--;
            }
            else if (targetIndex == 0)
            {
                // Cycle back to last targetIndex
                targetIndex = enemies->size() - 1;;
            }

            iprintf("CurTar: ");
            iprintf(enemies->at(targetIndex)->name.c_str());
            iprintf("\n");

            
        }
        else if (keys & KEY_RIGHT)
        {
            if (targetIndex < enemies->size() - 1)
            {
                targetIndex++;
            }
            else if (targetIndex == enemies->size() - 1)
            {
                targetIndex = 0;
            }

            iprintf("CurTar: ");
            iprintf(enemies->at(targetIndex)->name.c_str());
            iprintf("\n");
        }

        if (keys & KEY_A)
        {
            iprintf("Attacking: ");
            iprintf(enemies->at(targetIndex)->name.c_str());
            enemies->at(targetIndex)->hp -= player->baseAttack->calculateDamage(&player->ma, &player->st, &enemies->at(targetIndex)->en, &player->lv, &enemies->at(targetIndex)->lv);
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
            
            inProgress = false;
    }

        if (keys & KEY_B)
        {
            inProgress = false;
        }
    }
}