#include "SwitchPersona.h"

BattleResult SwitchPersona::resolve(PartyMember* /*user*/, BattleParticipant* /*target*/, Skill* /*skill*/)
{
    return { false, 0, false, "" };
}
