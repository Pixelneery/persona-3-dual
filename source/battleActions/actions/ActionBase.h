#pragma once
#include <nds.h>
#include <stdio.h>
#include <string>
#include "../ParticipantType.h"
#include "../BattleParticipant.h"
#include "../party/PartyMember.h"

struct ActionBase
{
    bool canUse = false;
    bool inProgress = false;
    u32 targetIndex = 0;
    std::string name = "";
    ParticipantType possibleUsers;

    std::vector<BattleParticipant *> *allParticipants;
    std::vector<BattleParticipant *> *party;
    std::vector<BattleParticipant *> *enemies;

    virtual void execute() = 0;
    virtual bool update(u32 *keys, PartyMember *user) = 0;

    ActionBase(std::vector<BattleParticipant *> *iAllParticipants, std::vector<BattleParticipant *> *iParty, std::vector<BattleParticipant *> *iEnemies)
        : allParticipants(iAllParticipants), party(iParty), enemies(iEnemies) {}
    virtual ~ActionBase() = default;
};
