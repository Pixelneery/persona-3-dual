#include "HealSkill.h"
#include "../BattleParticipant.h"
#include <algorithm>

const float HealSkill::magicBoostTable[20] = {
    0,   // 1-5
    6,   // 6-10
    12,  // 11-15
    17,  // 16-20
    22,  // 21-25
    27,  // 26-30
    34,  // 31-35
    44,  // 36-40
    54,  // 41-45
    65,  // 46-50
    75,  // 51-55
    85,  // 56-60
    93,  // 61-65
    100, // 66-70
    105, // 71-75
    110, // 76-80
    115, // 81-85
    120, // 86-90
    125, // 91-95
    130  // 96-99
};

u32 HealSkill::calculateHealing(BattleParticipant& user)
{
    float teamMultiplier;
    BattleStats* battleStats;
    if (user.participantType == ParticipantType::Party || user.participantType == ParticipantType::Player)
    {
        teamMultiplier = 1.0f;
        PartyMember& partyMember = static_cast<PartyMember&>(user);
        battleStats = &partyMember.curPersona->battleStats;
    }
    else
    {
        teamMultiplier = 0.6f;
        Enemy& enemy = static_cast<Enemy&>(user);
        battleStats = &enemy.battleStats;
    }

    u32 magicBoost = getMagicBoost(battleStats->ma);
    u32 base = (u32)floor((movePower + magicBoost) * teamMultiplier);
    u32 range = 95 + (u32)(rand() % 11);
    return (u32)floor(base * range / 100.0f);
}

u32 HealSkill::getMagicBoost(u32& magic)
{
    u32 index = (magic - 1) / 5;
    index = std::clamp(index, (u32)0, (u32)19);
    return magicBoostTable[index];
}
