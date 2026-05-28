#pragma once
#include <nds.h>
#include <vector>
#include "skills/AttackSkill.h"
#include "ParticipantType.h"

struct BattleParticipant
{
    std::string name;
    s32 hp;
    s32 sp;
    u32 lv;
    // TODO: dont think so that all bosses have a basattack, possibly move in the future
    AttackSkill *baseAttackAction;
    ParticipantType participantType;

    float currentTurnOrderAgility;
    bool oneMore = false;
    // knocked will be needed in the future, no use yet
    bool knockedDown = false;
};
