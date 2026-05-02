#include "BattleController.h"
#include <nds.h>
#include <cstdlib>
#include <ctime>

void BattleController::execute()
{
    char str1[1];
    std::sprintf(str1, "size: %d", enemies->size());
    iprintf(str1);

    exited = false;
    ActionBase *actions[] = {&attack, &guard, &persona};
    while (!exited)
    {
        scanKeys();

        u32 keys = keysDown();
        if (keys & KEY_LEFT)
        {
            if (index > 0)
            {
                index--;
            }
            else if (index == 0)
            {
                // Cycle back to last index
                index = (sizeof(actions) / 4) - 1;
            }

            iprintf("Cur: ");
            iprintf(actions[index]->name.c_str());
            iprintf("\n");
        }
        else if (keys & KEY_RIGHT)
        {
            if (index < (sizeof(actions) / 4) - 1)
            {
                index++;
            }
            else if (index == (sizeof(actions) / 4) - 1)
            {
                index = 0;
            }

            iprintf("Cur: ");
            iprintf(actions[index]->name.c_str());
            iprintf("\n");
        }

        if (keys & KEY_A)
        {
            actions[index]->execute();
            if (enemies->empty())
            {
                exited = true;
                continue;
            }  

            enemyTurn();
        }

        if (keys & KEY_SELECT)
        {
            exited = true;
        }
    }
}

void BattleController::enemyTurn()
{
    for (int i = 0; i < enemies->size(); i++)
    {
        srand(time(0));
        int randomNum = rand() % enemies->at(i)->attackCount;
        player.hp -= enemies->at(i)->attacking[randomNum]->calculateDamage(&enemies->at(i)->ma, &enemies->at(i)->st, &player.en, &enemies->at(i)->lv, &player.lv);

        iprintf("Attacking with: ");
        iprintf(enemies->at(i)->attacking[randomNum]->name.c_str());
        iprintf("\n");

        char str[50];
        std::sprintf(str, "remaing player hp: %d \n", player.hp);
        iprintf(str);

        if (player.hp <= 0)
        {
            // idk just resets for now
            player.hp = 72;
            exited = true;
            break;
        }
    }
}

BattleController::BattleController() : attack(enemies, &player), guard(&player), persona(enemies, &player) {}
BattleController::~BattleController() {}
