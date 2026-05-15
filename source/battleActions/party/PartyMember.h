#pragma once
#include <nds.h>
#include "../personas/PersonaBase.h"
#include "../ArmourType.h"
#include "../armours/Armour.h"
#include "../BattleParticipant.h"

struct PartyMember : BattleParticipant
{
    ArmourType armourType;
    Armour armour;
    PersonaBase *persona;

    bool guarding = false;
};
