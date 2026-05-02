#pragma once

#include <stdio.h>
#include <nds.h>
#include <stdio.h>
#include <vector>

#include "./battleActions/ActionBase.h"
#include "./battleActions/Attack.h"
#include "./battleActions/Guard.h"
#include "./battleActions/Persona.h"
#include "./battleActions/Party/curPlayer.h"
#include "./battleActions/Enemies/Enemy.h"
#include "./battleActions/Enemies/Cowardly_Maya.h"
#include "./battleActions/Enemies/Merciless_Maya.h"

class BattleController
{
private:

    bool exited = false;
    u32 index = 0;
   
    //Battle participants
    curPlayer player;
    Cowardly_Maya cowardly_Maya;
    Merciless_Maya merciless_Maya;
    //TODO: dont forget to clear in future
    std::vector<Enemy*>* enemies = new std::vector<Enemy*>({&merciless_Maya, &cowardly_Maya});

    //actions, DO NOT MOVE ABOVE BATTLE PARTICIPANTS DECLERATIONS
    Attack attack;
    Guard guard;
    Persona persona;

    void enemyTurn();

public:
    BattleController();
    ~BattleController();

    void execute();
};
