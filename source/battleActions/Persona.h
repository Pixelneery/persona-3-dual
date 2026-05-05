#pragma once
#include <stdio.h>
#include <vector>

#include "ActionBase.h"
#include "Enemies/Enemy.h"
#include "party/player.h"
#include "TargetAndAttackEnemy.h"
#include "UpdateIndex.h"



struct Persona : ActionBase
{
    UpdateIndex updateIndex;
    std::vector<Enemy *> *enemies = new std::vector<Enemy *>;
    Player *player;
    AttackSkill *selectedSkill;

    Persona(std::vector<Enemy *> *iEnemies, Player *iPlayer) : enemies(iEnemies), player(iPlayer)
    {
        name = "Persona";
    }


  //  TargetAndAttackActionEnemy targetAndAttackActionEnemy(player.);


    enum MenuState  {
        SelectSkill,
        SelectTarget,
    };
    MenuState menuState;

   
    void execute() override;
    bool update() override;
};
