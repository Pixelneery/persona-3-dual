#pragma once
#include "ActionBase.h"
#include "Enemies/Enemy.h"
#include "Party/player.h"
#include <stdio.h>
#include <vector>

struct Attack : ActionBase 
{
    
    std::vector<Enemy*>* enemies;
    Player* player;

    Attack(std::vector<Enemy*>* iEnemies, Player* iPlayer) : enemies(iEnemies), player(iPlayer)
    {
        name = "Attack";
    }

    void execute() override;
};
