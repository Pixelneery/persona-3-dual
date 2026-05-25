#pragma once
#include <nds.h>
#include <string>

struct BattleResult
{
    bool hit = false;
    s32 hpDelta = 0;
    bool oneMore = false;
    std::string log;
};
