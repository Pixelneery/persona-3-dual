#pragma once
#include <nds.h>
// if you  think this should have a cpp file let me know

struct UpdateIndex
{

    void update(u32 &keys, u32 &index, u32 &structureSize)
    {
        if (keys & KEY_LEFT)
        {
            if (index > 0)
            {
                index--;
            }
            else if (index == 0)
            {
                // Cycle back to last index
                index = structureSize - 1;
            }
        }

        else if (keys & KEY_RIGHT)
        {
            if (index < structureSize - 1)
            {
                index++;
            }
            else if (index == structureSize - 1)
            {
                index = 0;
            }
        }
    }
};
