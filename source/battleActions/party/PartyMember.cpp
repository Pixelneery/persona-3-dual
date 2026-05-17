#include "PartyMember.h"
#include "../actions/ActionBase.h"

void PartyMember::Init(std::array<ActionBase *, 4> *iActions)
{
    actions = iActions;
    if (participantType == ParticipantType::Player)
    {
        for (int i = 0; i < actions->size(); i++)
        {
            canUseAction[i] = true;
        }
    }
    else if (participantType == ParticipantType::Party)
    {
        for (int i = 0; i < actions->size(); i++)
        {
            if (actions->at(i)->possibleUsers == ParticipantType::Party)
            {
                canUseAction[i] = true;
            }
            else
            {
                canUseAction[i] = false;
            }
        }
    }
}

bool PartyMember::TakeTurn(u32 *keys)
{
    u32 actionCount = actions->size();
    for (u32 i = 0; i < actionCount; i++)
    {
        if (actions->at(i)->inProgress)
        {
            return actions->at(i)->update(keys, this);
        }
    }

    updateIndex.update(*keys, index, actionCount);

    if (*keys & KEY_LEFT || *keys & KEY_RIGHT)
    {
        iprintf("Cur: ");
        iprintf(actions->at(index)->name.c_str());
        iprintf("\n");
    }

    if (*keys & KEY_A && canUseAction[index])
    {
        actions->at(index)->execute();
    }

    return false;
}
