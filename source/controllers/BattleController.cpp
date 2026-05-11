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

    if (counter < enemies->size() && isEnemyTurn)
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
                    for (u32 i = 0; i < enemies->size(); i++)
                        if (enemies->at(i)->hp <= 0)
                        {
                            enemies->erase(enemies->begin() + i);
                            counter = 0;
                            i--;
                        }

                    if (enemies->empty())
                        exit();

                    if (!player->oneMore)
                    {
                        isEnemyTurn = true;
                    }
                    else
                    {
                        player->oneMore = false;
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
    if (enemies->at(counter)->knockedDown)
    {
        // recovery logic in the future
        enemies->at(counter)->knockedDown = false;
    }

    srand(time(0));
    int randomNum = rand() % enemies->at(counter)->attackCount;

    AttackSkill *curSkill = enemies->at(counter)->attackSkill[randomNum];

    u32 damage = curSkill->calculateDamageEnemySkill(enemies->at(counter)->getBattleStats(), player->curPersona->getBattleStats(), &enemies->at(counter)->lv, &player->lv, &player->armour);
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
        player->hp -= (s32)damage;
        iprintf("Attack with: ");
        iprintf(curSkill->name.c_str());
        iprintf("\n");
        char str[50];
        std::sprintf(str, "remaining player hp: %ld \n", player->hp);
        iprintf(str);
        if (player->hp <= 0)
        {
            player->hp = 72;
            counter = 0;
            exit();
            return;
        }

        u32 affinity = player->curPersona->affinities[curSkill->element];
        if (affinity == BattleStats::Affinity::Weak && !player->knockedDown && !player->guarding)
        {
            enemies->at(counter)->oneMore = true;
            iprintf("one more!\n");
            player->knockedDown = true;
        }

        if (!enemies->at(counter)->oneMore)
        {
            counter++;
        }
        else
        {
            enemies->at(counter)->oneMore = false;
        }
    }

    if (counter >= enemies->size())
    {
        player->guarding = false;
        if (player->knockedDown)
        {
            // recovery logic in the future
            player->knockedDown = false;
        }
        isEnemyTurn = false;
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
