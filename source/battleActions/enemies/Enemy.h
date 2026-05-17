#pragma once
#include <nds.h>
#include <string>
#include "../skills/AttackSkill.h"
#include "../party/PartyMember.h"
#include "../BattleParticipant.h"
#include "../DeductAttackCost.h"

/*
St	Represents strength and physical damage.
Ma	Represents magic and magical damage.
En	Represents endurance, which determines your defense and how much damage you can take.
Ag	Represents agility, which determines your place in the turn order.
Lu	Represents luck, which is taken into account when using certain skills involving status afflictions or insta-death abilities.*/

struct Enemy : BattleParticipant
{
    AttackSkill **attackSkill;
    u32 attackCount;
    // add arcana in the future

    Enemy()
    {
        participantType = ParticipantType::Enemy;
    }

    std::vector<BattleParticipant *> *enemies;
    std::vector<PartyMember *> *partyMembers;

    void Init(std::vector<BattleParticipant *> *iEnemies, std::vector<PartyMember *> *iPartyMembers);

    // enemys have indivual ais in the future
    bool TakeTurn(u32 *keys) override;

    virtual ~Enemy() = default;
};
