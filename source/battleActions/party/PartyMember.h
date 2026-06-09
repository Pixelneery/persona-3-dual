#pragma once
#include "../BattleParticipant.h"
#include "../ParticipantType.h"
#include "../armours/Armour.h"
#include "../personas/PersonaBase.h"
#include "../shoes/Shoe.h"
#include "../weapons/Weapon.h"
#include "CharacterProfile.h"
#include <nds.h>

struct PartyMember : BattleParticipant
{
    ArmourType* armourType;
    std::vector<PersonaBase*> personas;
    PersonaBase* curPersona;
    WeaponType weaponType;
    Weapon weapon;

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
        weaponType = characterProfile->weaponType;
        weapon = characterProfile->weapon;
        personas = characterProfile->personas;
        curPersona = characterProfile->curPersona;
    }

    BattleStats* getBattleStats() override
    {
        return &curPersona->battleStats;
    }

    float calculateBaseDamage(BattleParticipant& defender, Skill& skill) override;

    ~PartyMember()
    {
    }
};
