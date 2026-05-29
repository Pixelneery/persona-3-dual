#pragma once
#include "ParticipantType.h"
#include "skills/AttackSkill.h"
#include <nds.h>
#include <vector>

struct BattleParticipant
{
    std::string name;
    s32 maxHp;
    s32 hp;
    s32 maxSp;
    s32 sp;
    u32 lv;
    // TODO: dont think so that all bosses have a basattack, possibly move in the future
    AttackSkill* baseAttackAction;
    ParticipantType participantType;

    float currentTurnOrderAgility;
    bool oneMore = false;
    // knocked will be needed in the future, no use yet
    bool knockedDown = false;
};
