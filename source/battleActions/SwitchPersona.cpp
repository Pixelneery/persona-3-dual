#include "SwitchPersona.h"

void SwitchPersona::execute()
{
    inProgress = true;
    targetIndex = 0;
}

bool SwitchPersona::update(u32 *keys)
{
    updateIndex.update(*keys, targetIndex, player->personaCount);

    if (*keys & KEY_LEFT || *keys & KEY_RIGHT)
    {
        iprintf("Cur: ");
        iprintf(player->personas[targetIndex]->name.c_str());
        iprintf("\n");
    }

    if (*keys & KEY_A)
    {
        if (player->curPersona == player->personas[targetIndex])
        {
            iprintf("already selected this Persona\n");
            return false;
        }
        iprintf("Sel: ");
        player->curPersona = player->personas[targetIndex];
        iprintf(player->curPersona->name.c_str());
        iprintf("\n");

        inProgress = false;
        return true;
    }

    if (*keys & KEY_B)
    {
        inProgress = false;
    }

    return false;
}
