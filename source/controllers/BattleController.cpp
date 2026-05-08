#include "BattleController.h"
#include <nds.h>
#include <cstdlib>
#include <ctime>
#include <array>

void BattleController::execute()
{
    if (enemies == nullptr)
    {
        iprintf("enemies null\n");
        return;
    }

    active = true;
    actions = {&attack, &guard, &persona};

    counter = 0;
}

void BattleController::update(u32 keys)
{
    if (!active)
    {
        return;
    }

    if (counter < enemies->size() && isEnemeyTurn)
    {
        enemyTurn();
        return;
    }

    u32 actionCount = actions.size();
    for (u32 i = 0; i < actionCount; i++)
    {
        if (actions[i]->inProgress)
        {
            bool madeAction = actions[i]->update();
            if (!actions[i]->inProgress)
            {
                if (enemies->size() == 0)
                {
                    exit();
                }
                else if (madeAction)
                {
                    isEnemeyTurn = true;
                }
            }
            return;
        }
    }

    updateIndex.update(keys, index, actionCount);

    if (keys & KEY_LEFT)
    {
        iprintf("Cur: ");
        iprintf(actions[index]->name.c_str());
        iprintf("\n");
    }
    else if (keys & KEY_RIGHT)
    {
        iprintf("Cur: ");
        iprintf(actions[index]->name.c_str());
        iprintf("\n");
    }

    if (keys & KEY_A)
    {
        actions[index]->execute();
        if (enemies->empty())
        {
            exit();
        }
    }
}

void BattleController::enemyTurn()
{
    srand(time(0));

    int randomNum = rand() % enemies->at(counter)->attackCount;
    u32 damage = enemies->at(counter)->attackSkill[randomNum]->calculateDamage(&enemies->at(counter)->ma, &enemies->at(counter)->st, &player->en, &enemies->at(counter)->lv, &player->lv);
    if (player->guarding)
    {
        iprintf("player guarded\n");
        damage *= 0.4;
    }

    player->hp -= damage;

    iprintf("Attack with: ");
    iprintf(enemies->at(counter)->attackSkill[randomNum]->name.c_str());
    iprintf("\n");

    char str[50];
    std::sprintf(str, "remaing player hp: %lu \n", player->hp);
    iprintf(str);

    if (player->hp <= 0)
    {
        // idk just resets for now
        player->hp = 72;
        counter = 0;
        exit();
    }
    else
    {
        counter++;
    }

    if (counter >= enemies->size())
    {
        player->guarding = false;
        isEnemeyTurn = false;
        counter = 0;
    }
}

void BattleController::exit()
{
    actions[index]->inProgress = false;
    consoleClear();
    active = false;
}

BattleController::BattleController(Player *iPlayer, std::vector<Enemy *> *iEnemies) : player(iPlayer), enemies(iEnemies), attack(enemies, player), guard(player), persona(enemies, player) {}
