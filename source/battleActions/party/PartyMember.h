#pragma once
#include <nds.h>
#include <array>
#include "../personas/PersonaBase.h"
#include "../ArmourType.h"
#include "../armours/Armour.h"
#include "../BattleParticipant.h"
#include "../ParticipantType.h"
#include "./battleActions/UpdateIndex.h"
#include "CharacterProfile.h"

struct ActionBase;

struct PartyMember : BattleParticipant
{
    UpdateIndex updateIndex;
    u32 index = 0;

    Armour armour;
    std::vector<PersonaBase *> personas;
    PersonaBase *curPersona;

    bool guarding = false;

    CharacterProfile *characterProfile;

    bool canUseAction[4];
    std::array<ActionBase *, 4> *actions = nullptr;

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
    ~PartyMember() {};

    void Init(std::array<ActionBase *, 4> *iActions);
    bool TakeTurn(u32 *keys) override;
};
