#pragma once
#include <nds.h>
#include <stdio.h>
#include <vector>
#include "skills/AttackSkill.h"
#include "enemies/Enemy.h"
#include "party/Player.h"
#include "DeductAttackCost.h"

struct TargetAndAttackActionEnemy
{
    TargetAndAttackActionEnemy(std::vector<Enemy *> *iEnemies, Player *iPlayer, u32 *iTargetIndex) : enemies(iEnemies), player(iPlayer), targetIndex(iTargetIndex) {}

    std::vector<Enemy *> *enemies;
    Player *player;
    u32 *targetIndex;

    bool update(u32 *keys, AttackSkill *attack);
};
