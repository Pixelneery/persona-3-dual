#pragma once
#include "ActionBase.h"
#include "enemies/Enemy.h"
#include "party/Player.h"
#include <stdio.h>
#include <vector>
#include "UpdateIndex.h"

struct AttackAction : ActionBase 
{
    UpdateIndex updateIndex;
    std::vector<Enemy*>* enemies;
    Player* player;

    AttackAction(std::vector<Enemy*>* iEnemies, Player* iPlayer) : enemies(iEnemies), player(iPlayer)
    {
        name = "AttackAction";
    }

    void execute() override;
    bool update() override;
};
