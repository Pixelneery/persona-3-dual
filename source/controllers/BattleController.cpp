#include "BattleController.h"
#include <nds.h>
#include <cstdlib>
#include <ctime>
#include <array>

void BattleController::execute()
{
    active = true;

    // TODO: method for selecting party members in the future
    player = new PartyMember(&characterProfiles->player);
    yukari = new PartyMember(&characterProfiles->yukari);
    junpei = new PartyMember(&characterProfiles->junpei);

    player->Init(&actions);
    yukari->Init(&actions);
    junpei->Init(&actions);
    battleParticipants->push_back(player);
    battleParticipants->push_back(yukari);
    battleParticipants->push_back(junpei);
    for (BattleParticipant *participant : *battleParticipants)
    {
        if (participant->participantType == ParticipantType::Enemy)
        {
            enemies.push_back(participant);
        }
        else if (participant->participantType == ParticipantType::Party || participant->participantType == ParticipantType::Player)
        {
            partyMembers.push_back(participant);
        }
    }
    for (BattleParticipant *participant : enemies)
    {
        static_cast<Enemy *>(participant)->Init(&enemies, &partyMembers);
    }
}

void BattleController::update(u32 keys)
{
    if (!active)
        return;
    currentParticipantTurn = battleParticipants->at(turnsTaken % battleParticipants->size());
    bool turnResult = currentParticipantTurn->TakeTurn(&keys);
    if (turnResult)
    {
        iprintf("Previous attaker: ");
        iprintf(currentParticipantTurn->name.c_str());
        iprintf("\n");

        for (u32 i = 0; i < battleParticipants->size(); i++)
        {

            if (battleParticipants->at(i)->hp <= 0)
            {
                // adjusts the index for the next round so it wont skip an entry
                turnsTaken--;
                BattleParticipant *dead = battleParticipants->at(i);

                if (dead->participantType == ParticipantType::Player)
                {
                    exit();
                }
                else if (dead->participantType == ParticipantType::Enemy)
                {
                    for (u32 j = 0; j < enemies.size(); j++)
                    {
                        if (enemies.at(j) == dead)
                        {
                            enemies.erase(enemies.begin() + j);
                            break;
                        }
                    }

                    if (enemies.size() == 0)
                        exit();
                }

                else if (dead->participantType == ParticipantType::Party)
                {
                    for (u32 j = 0; j < partyMembers.size(); j++)
                    {
                        if (partyMembers.at(j) == dead)
                        {
                            partyMembers.erase(partyMembers.begin() + j);
                            break;
                        }
                    }

                    if (partyMembers.size() == 0)
                        exit();
                }

                battleParticipants->erase(battleParticipants->begin() + i);
                i--;
            }
        }

        if (!currentParticipantTurn->oneMore)
            turnsTaken++;
        else
        {
            currentParticipantTurn->oneMore = false;
        }
    }
}

void BattleController::exit()
{
    // TODO: proper exit
    //  actions[index]->inProgress = false;
    consoleClear();
    active = false;
}

BattleController::BattleController(std::vector<BattleParticipant *> *iBattleParticipant, CharacterProfiles *iCharacterProfiles)
    : battleParticipants(iBattleParticipant),
      characterProfiles(iCharacterProfiles),
      attack(battleParticipants, &partyMembers, &enemies),
      guard(battleParticipants, &partyMembers, &enemies),
      persona(battleParticipants, &partyMembers, &enemies),
      switchPersona(battleParticipants, &partyMembers, &enemies) {}
