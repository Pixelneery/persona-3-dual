#pragma once
#include "CharacterProfile.h"

#include "../personas/Forneus.h"
#include "../personas/Hermes.h"
#include "../personas/Io.h"
#include "../personas/Orpheus.h"

#include "../skills/Pierce_Attack.h"
#include "../skills/Slash_Attack.h"

#include "../armours/PlainShirt.h"
#include "../armours/RashGuard.h"
#include "../shoes/RubberSole.h"

struct CharacterProfiles
{
    CharacterProfile player;
    CharacterProfile yukari;
    CharacterProfile junpei;

    void InitializeProfiles();
};
