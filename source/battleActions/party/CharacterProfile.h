#pragma once
#include <nds.h>
#include <vector>
#include "../skills/AttackSkill.h"
#include "../ParticipantType.h"
#include "../personas/PersonaBase.h"

struct CharacterProfile
{
    std::string name;
    s32 hp;
    s32 sp;
    u32 lv;
    ParticipantType participantType;

    ArmourType armourType;
    Armour armour;
    AttackSkill *baseAttackAction;
    std::vector<PersonaBase *> personas;

    PersonaBase *curPersona;
};
