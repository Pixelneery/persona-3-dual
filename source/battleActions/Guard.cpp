#include "Guard.h"
#include <stdio.h>

void Guard::execute()
{
    inProgress = true;
    iprintf("Guarding\n");
}

bool Guard::update()
{
    player->guarding = true;
    inProgress = false;
    return true;
}
