#pragma once
#include "ActionBase.h"
#include "party/player.h"

struct Guard : ActionBase 
{
    Player* player;
    Guard(Player* iPlayer) : player(iPlayer)
    {
        name = "Guard";
    }

    void execute() override;
    bool update() override;
};
