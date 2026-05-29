#pragma once
#include "../BattleParticipant.h"
#include "../ParticipantType.h"
#include "../armours/Armour.h"
#include "../personas/PersonaBase.h"
#include "../shoes/Shoe.h"
#include "CharacterProfile.h"
#include <nds.h>

struct PartyMember : BattleParticipant
{
    ArmourType* armourType;
    Armour* armour;
    Shoe* shoe;
    std::vector<PersonaBase*> personas;
    PersonaBase* curPersona;

    bool guarding = false;

    CharacterProfile* characterProfile;

    PartyMember(CharacterProfile* iCharacterProfile) : characterProfile(iCharacterProfile)
    {
        name = characterProfile->name;
        maxHp = characterProfile->maxHp;
        hp = characterProfile->hp;
        maxSp = characterProfile->maxSp;
        sp = characterProfile->sp;
        lv = characterProfile->lv;

        baseAttackAction = characterProfile->baseAttackAction;
        participantType = characterProfile->participantType;

        armourType = &characterProfile->armourType;
        armour = &characterProfile->armour;
        shoe = &characterProfile->shoe;
        personas = characterProfile->personas;
        curPersona = characterProfile->curPersona;
    }
    ~PartyMember()
    {
    }
};
