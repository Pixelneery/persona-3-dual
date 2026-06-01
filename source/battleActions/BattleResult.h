#pragma once
#include <array>
#include <nds.h>
#include <string>

//might as well use arrays to avoid overhead when theres a max of 5 targets
struct BattleResult
{
    u32 targetCount = 0;
    std::array<bool, 5> hit = {false, false, false, false, false};
    std::array<s32, 5> hpDelta = {0, 0, 0, 0, 0};
    std::array<bool, 5> oneMore = {false, false, false, false, false};
    std::array<std::string, 5> log;
};
