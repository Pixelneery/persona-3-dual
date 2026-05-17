#pragma once
#include "CharacterProfile.h"

#include "../personas/Orpheus.h"
#include "../personas/Forneus.h"
#include "../personas/Io.h"
#include "../personas/Hermes.h"

#include "../skills/Slash_Attack.h"
#include "../skills/Pierce_Attack.h"

#include "../armours/PlainShirt.h"
#include "../armours/RashGuard.h"

namespace CharacterProfiles
{
    extern CharacterProfile player;
    extern CharacterProfile yukari;
    extern CharacterProfile junpei;

    void InitializeProfiles();
}
