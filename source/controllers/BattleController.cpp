#include "BattleController.h"
#include <nds.h>
#include <cstdlib>
#include <ctime>
#include <array>

void BattleController::execute()
{
    active = true;

    for (BattleParticipant *participant : *battleParticipants)
    {
        if (participant->participantType == ParticipantType::Enemy)
        {
            enemies->push_back(participant);
        }
        else if (participant->participantType == ParticipantType::Party || participant->participantType == ParticipantType::Player)
        {
            partyMembers->push_back(participant);
        }
    }

    player->Init(&actions);
    yukari->Init(&actions);
    junpei->Init(&actions);

    // dont harcode, move
    battleParticipants->push_back(player);
    battleParticipants->push_back(yukari);
    battleParticipants->push_back(junpei);
}

void BattleController::update(u32 keys)
{
    if (!active)
    {
        return;
    }

    currentParticipantTurn = battleParticipants->at(turnsTaken % battleParticipants->size());

    bool turnResult = currentParticipantTurn->TakeTurn(&keys);

    u8 enemyCounter;
    u8 partyCounter;

    if (turnResult)
    {
        // properly erase dead participants, will be changed in the future bcs of revives etc
        for (u32 i = 0; i < battleParticipants->size(); i++)
            if (battleParticipants->at(i)->hp <= 0)
            {
                // TODO: find way to actually end battle
                battleParticipants->erase(battleParticipants->begin() + i);
                i--;
            }

        if (!currentParticipantTurn->oneMore)
        {
            turnsTaken++;
        }
        else
        {
            currentParticipantTurn->oneMore = false;
        }
    }
    /*
    if (counter < enemies->size() && isEnemyTurn)
    {
        enemyTurn();
        return;
    }
    */
}

void BattleController::exit()
{
    // TODO: proper exit
    //  actions[index]->inProgress = false;
    consoleClear();
    active = false;
}

BattleController::BattleController(std::vector<BattleParticipant *> *iBattleParticipant)
    : battleParticipants(iBattleParticipant),
      attack(battleParticipants, partyMembers, enemies),
      guard(battleParticipants, partyMembers, enemies),
      persona(battleParticipants, partyMembers, enemies),
      switchPersona(battleParticipants, partyMembers, enemies) {}
