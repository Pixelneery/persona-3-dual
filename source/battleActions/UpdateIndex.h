#pragma once
#include <nds.h>

struct UpdateIndex
{
    void update(u32 &keys, u32 &index, u32 structureSize)
    {
        if (structureSize == 0)
            return; // Prevent division by zero

        if (keys & KEY_LEFT)
            index = (index + structureSize - 1) % structureSize;
        else if (keys & KEY_RIGHT)
            index = (index + 1) % structureSize;
    }

    void updateSkipDead(u32 &keys, u32 &index, std::vector<BattleParticipant *> &battleParticipants)
    {
        if (keys & KEY_LEFT || keys & KEY_RIGHT)
        {
            u32 next = index;
            do
            {
                if (keys & KEY_LEFT)
                    next = (next + battleParticipants.size() - 1) % battleParticipants.size();
                else if (keys & KEY_RIGHT)
                    next = (next + 1) % battleParticipants.size();
            } while (battleParticipants[next]->hp <= 0);

            index = next;
        }
    }
};
