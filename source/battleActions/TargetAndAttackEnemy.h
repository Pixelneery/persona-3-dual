#pragma once 
#include <nds.h>
#include <stdio.h> 
#include <vector>

#include "skills/AttackSkill.h"
#include "enemies/Enemy.h"
#include "party/Player.h"

struct TargetAndAttackActionEnemy
{
    Player* player;
    std::vector<Enemy*>* enemies;
    u32 *targetIndex; 

    TargetAndAttackActionEnemy(std::vector<Enemy*>* iEnemies, Player *iPlayer, u32 *iTargetIndex) :
    enemies(iEnemies), targetIndex(iTargetIndex), player(iPlayer) {}

    bool update(u32* keys, AttackSkill* attack);
};
