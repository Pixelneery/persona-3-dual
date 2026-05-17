#include "Guard.h"
#include <stdio.h>

void Guard::execute()
{
    inProgress = true;
    iprintf("Guarding\n");
}

bool Guard::update(u32 *keys, PartyMember *user)
{
    user->guarding = true;
    inProgress = false;
    return true;
}
