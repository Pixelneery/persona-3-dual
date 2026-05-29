#include "SwitchPersona.h"

void SwitchPersona::execute()
{
    inProgress = true;
    targetIndex = 0;
}

bool SwitchPersona::update(u32 *keys, PartyMember *user)
{
    updateIndex.update(*keys, targetIndex, user->personas.size());

    if (*keys & KEY_LEFT || *keys & KEY_RIGHT)
    {
        iprintf("Cur: ");
        iprintf(user->personas[targetIndex]->name.c_str());
        iprintf("\n");
    }

    if (*keys & KEY_A)
    {
        if (user->curPersona == user->personas[targetIndex])
        {
            iprintf("already selected this Persona\n");
            return false;
        }
        iprintf("Sel: ");
        user->curPersona = user->personas[targetIndex];
        iprintf(user->curPersona->name.c_str());
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
