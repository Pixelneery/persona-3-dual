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
    actions = {&attack, &guard, &persona, &switchPersona};

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
            bool madeAction = actions[i]->update(&keys);
            if (!actions[i]->inProgress)
            {
                if (enemies->size() == 0)
                {
                    exit();
                }
                else if (madeAction)
                {
                    // properly erase dead enemies
                    for (int i = 0; i < enemies->size(); i++)
                        if (enemies->at(i)->hp <= 0)
                        {
                            enemies->erase(enemies->begin() + i);
                            counter = 0;
                            i--;
                        }

                    if (enemies->empty())
                        exit();
                    else
                    {
                        isEnemeyTurn = true;
                    }
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

    AttackSkill *curSkill = enemies->at(counter)->attackSkill[randomNum];

    u32 damage = curSkill->calculateDamage(&enemies->at(counter)->ma, &enemies->at(counter)->st, &player->en, &enemies->at(counter)->lv, &player->lv);
    if (player->guarding)
    {
        iprintf("player guarded\n");
        damage *= 0.4;
    }

    bool attacked = false;
    if (curSkill->race == AttackSkill::mag)
    {
        if (!DeductAttackCost(&enemies->at(counter)->sp, curSkill->cost, "not enough SP\n"))
        {
            counter++;
        }
        else
        {
            attacked = true;
        }
    }
    else if (curSkill->race == AttackSkill::phys)
    {
        if (!DeductAttackCost(&enemies->at(counter)->hp, curSkill->cost, "not enough HP\n"))
        {
            counter++;
        }
        else
        {
            attacked = true;
        }
    }

    if (attacked)
    {
        player->hp -= damage;
        iprintf("Attack with: ");
        iprintf(curSkill->name.c_str());
        iprintf("\n");
        char str[50];
        std::sprintf(str, "remaining player hp: %lu \n", player->hp);
        iprintf(str);
        if (player->hp <= 0)
        {
            player->hp = 72;
            counter = 0;
            exit();
            return;
        }
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

BattleController::BattleController(Player *iPlayer, std::vector<Enemy *> *iEnemies) : player(iPlayer), enemies(iEnemies), attack(enemies, player), guard(player), persona(enemies, player), switchPersona(player) {}
