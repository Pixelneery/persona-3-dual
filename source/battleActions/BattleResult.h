#pragma once
#include <nds.h>
#include <string>

struct BattleResult {
    bool hit     = false;
    s32  hpDelta = 0;    // negative = damage dealt, positive = HP healed
    bool oneMore = false;
    std::string log;     // human-readable description of what happened
};
