#pragma once
#include <nds.h>
#include "../personas/PersonaBase.h"
#include "../armours/Armour.h"
#include "../shoes/Shoe.h"
#include "../BattleParticipant.h"
#include "../ParticipantType.h"
#include "CharacterProfile.h"

struct PartyMember : BattleParticipant
{
    Armour armour;
    Shoe shoe;
    std::vector<PersonaBase *> personas;
    PersonaBase *curPersona;

    bool guarding = false;

    CharacterProfile *characterProfile;

    PartyMember(CharacterProfile *iCharacterProfile) : characterProfile(iCharacterProfile)
    {
        name = characterProfile->name;
        hp = characterProfile->hp;
        sp = characterProfile->sp;
        lv = characterProfile->lv;

        baseAttackAction = characterProfile->baseAttackAction;
        participantType = characterProfile->participantType;

        armour = characterProfile->armour;
        personas = characterProfile->personas;
        curPersona = characterProfile->curPersona;
    }
    ~PartyMember() {}
};
