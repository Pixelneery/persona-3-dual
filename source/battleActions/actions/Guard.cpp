#include "Guard.h"

BattleResult Guard::resolve(PartyMember* user, BattleParticipant* /*target*/, Skill* /*skill*/)
{
    user->guarding = true;
    return {true, 0, false, "Guard"};
}
