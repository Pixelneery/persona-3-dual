#pragma once
#include <stdio.h>

#include "ActionBase.h"
#include "party/Player.h"
#include "UpdateIndex.h"

struct SwitchPersona : ActionBase
{
    UpdateIndex updateIndex;
    Player *player;

    SwitchPersona(Player *iPlayer) : player(iPlayer)
    {
        name = "SwitchPersona";
    }

    void execute() override;
    bool update(u32 *keys) override;
};
