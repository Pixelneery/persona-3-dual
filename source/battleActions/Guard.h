#pragma once
#include "ActionBase.h"
#include "Party/player.h"

struct Guard : ActionBase 
{
    Player* player;
    Guard(Player* iPlayer) : player(iPlayer)
    {
        name = "Guard";
    }

    void execute() override;
};
