#pragma once

#include <stdio.h>
#include <nds.h>
#include <stdio.h>
#include <vector>

#include "./battleActions/actions/AttackAction.h"
#include "./battleActions/actions/Guard.h"
#include "./battleActions/actions/PersonaAction.h"
#include "./battleActions/actions/SwitchPersona.h"

#include "./battleActions/BattleParticipant.h"
#include "./battleActions/enemies/Enemy.h"
#include "./battleActions/party/PartyMember.h"

#include "./battleActions/party/characterProfiles.h"

class BattleController
{
private:
    u32 turnsTaken = 0;
    BattleParticipant *currentParticipantTurn;
    bool active = false;
    bool isEnemyTurn = false;

    std::vector<BattleParticipant *> *battleParticipants;
    std::vector<BattleParticipant *> enemies;
    std::vector<BattleParticipant *> partyMembers;

    AttackAction attack;
    Guard guard;
    PersonaAction persona;
    SwitchPersona switchPersona;

    std::array<ActionBase *, 4> actions = {&attack, &guard, &persona, &switchPersona};

    CharacterProfiles *characterProfiles;
    // TODO: method for selecting party members in the future
    PartyMember *player = nullptr;
    PartyMember *yukari = nullptr;
    PartyMember *junpei = nullptr;

public:
    bool isActive() { return active; };
    void execute();
    void update(u32 keys);
    void exit();
    BattleController(std::vector<BattleParticipant *> *iBattleParticipants, CharacterProfiles *iCharacterProfiles);
    ~BattleController() {}
};
