#include "Enemy.h"
#include "../party/PartyMember.h"
#include "../skills/BattleCalcs.h"
#include <stdlib.h>

Skill* Enemy::pickSkill()
{
    u32 roll = rand() % (skillCount + 1);
    return (roll == 0) ? baseAttackAction : skill[roll - 1];
}

BattleParticipant* Enemy::pickTarget(std::vector<BattleParticipant*>& partyMembers)
{
    BattleParticipant* target = nullptr;
    do
    {
        target = partyMembers[rand() % partyMembers.size()];
    } while (target->hp <= 0);

    return target;
}

BattleResult Enemy::resolve(BattleParticipant* target, Skill* skill)
{
    PartyMember* party = static_cast<PartyMember*>(target);

    s32* resource;
    if (skill->skillRace == SkillRace::mag)
        resource = &sp;
    else
        resource = &hp;

    bool canAfford = *resource >= skill->cost;
    if (!canAfford)
        return {false, 0, false, skill->name};

    std::string targetLog = name + " targets " + target->name + "\n";

    *resource -= skill->cost;
    u32 accuracy = BattleCalcs::hitrate(*this, *target, *skill);
    bool hit = accuracy > u32(rand() % 100);

    if (!hit)
        return {false, 0, false, targetLog + "Miss"};

    u32 damage = BattleCalcs::attack(*this, *target, *skill);

    if (party->guarding)
        damage = (u32)(damage * 0.4f);

    bool oneMoreResult = false;
    u32 affinity = party->curPersona->battleStats.affinities[(u32)skill->element];
    if (affinity == BattleStats::Affinity::Weak && !party->knockedDown && !party->guarding)
    {
        oneMoreResult = true;
        party->knockedDown = true;
    }

    return {true, -(s32)damage, oneMoreResult, targetLog + skill->name};
}
