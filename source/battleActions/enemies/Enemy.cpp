#include "Enemy.h"
#include "../party/PartyMember.h"
#include <stdlib.h>

AttackSkill *Enemy::pickSkill()
{
    u32 roll = rand() % (attackCount + 1);
    return (roll == 0) ? baseAttackAction : attackSkill[roll - 1];
}

BattleParticipant *Enemy::pickTarget(std::vector<BattleParticipant *> &partyMembers)
{
    return partyMembers[rand() % partyMembers.size()];
}

BattleResult Enemy::resolve(BattleParticipant *target, AttackSkill *skill)
{
    PartyMember *party = static_cast<PartyMember *>(target);

    bool canAttack = false;
    if (skill->skillRace == SkillRace::mag)
        canAttack = DeductAttackCost(&sp, skill->cost);
    else
        canAttack = DeductAttackCost(&hp, skill->cost);

    if (!canAttack)
        return {false, 0, false, skill->name};

    std::string targetLog = name + " targets " + target->name + "\n";

    u32 accuracy = skill->calculateHitrateEnemy(&battleStats, &party->curPersona->battleStats, &party->shoe);
    bool hit = accuracy > u32(rand() % 100);

    if (!hit)
        return {false, 0, false, targetLog + "Miss"};

    u32 damage = (skill == baseAttackAction)
                     ? skill->calculateDamageEnemyRegular(&battleStats, &party->curPersona->battleStats, &lv, &target->lv, &party->armour)
                     : skill->calculateDamageEnemySkill(&battleStats, &party->curPersona->battleStats, &lv, &target->lv, &party->armour);

    if (party->guarding)
        damage = (u32)(damage * 0.4f);

    bool oneMoreResult = false;
    u32 affinity = party->curPersona->battleStats.affinities[skill->element];
    if (affinity == BattleStats::Affinity::Weak && !party->knockedDown && !party->guarding)
    {
        oneMoreResult = true;
        party->knockedDown = true;
    }

    target->hp -= (s32)damage;
    return {true, -(s32)damage, oneMoreResult, targetLog + skill->name};
}
