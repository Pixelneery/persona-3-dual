#pragma once
#include "ActionBase.h"
#include "Enemies/Enemy.h"
#include "Party/player.h"
#include <stdio.h>
#include <vector>

struct Persona : ActionBase
{
    std::vector<Enemy *> *enemies = new std::vector<Enemy *>;
    Player *player;

    Persona(std::vector<Enemy *> *iEnemies, Player *iPlayer) : enemies(iEnemies), player(iPlayer)
    {
        name = "Persona";
    }

    void execute() override;
};
