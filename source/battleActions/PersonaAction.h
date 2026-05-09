#pragma once
#include <stdio.h>
#include <vector>

#include "ActionBase.h"
#include "enemies/Enemy.h"
#include "party/Player.h"
#include "TargetAndAttackEnemy.h"
#include "UpdateIndex.h"

struct PersonaAction : ActionBase
{
    UpdateIndex updateIndex;
    std::vector<Enemy *> *enemies = new std::vector<Enemy *>;
    Player *player;
    AttackSkill *selectedSkill;
    TargetAndAttackActionEnemy *targetAndAttackActionEnemy;

    PersonaAction(std::vector<Enemy *> *iEnemies, Player *iPlayer) : enemies(iEnemies), player(iPlayer)
    {
        name = "Persona";
        // TODO: dont forget to clear in the future
        targetAndAttackActionEnemy = new TargetAndAttackActionEnemy(enemies, player, &targetIndex);
    }

    //  TargetAndAttackActionEnemy targetAndAttackActionEnemy(player.);

    enum MenuState
    {
        SelectSkill,
        SelectTarget,
    };
    MenuState menuState;

    void execute() override;
    bool update(u32 *keys) override;
};
