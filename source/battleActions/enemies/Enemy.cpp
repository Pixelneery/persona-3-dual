#include "Enemy.h"

void Enemy::Init(std::vector<BattleParticipant *> *iEnemies, std::vector<BattleParticipant *> *iPartyMembers)
{
    enemies = iEnemies;
    partyMembers = iPartyMembers;
}

bool Enemy::TakeTurn(u32 *keys)
{
    u32 randomNum = rand() % (attackCount + 1);

    PartyMember *target = static_cast<PartyMember *>(partyMembers->at(rand() % partyMembers->size()));

    u32 damage;
    AttackSkill *curSkill;

    // TODO: enemy ai. ugly way but this is temporary anyway since we dont have any actuall ai yet
    if (randomNum == 0)
    {
        curSkill = baseAttackAction;
        damage = curSkill->calculateDamageEnemyRegular(&battleStats, &target->curPersona->battleStats, &lv, &target->lv, &target->armour);
    }
    else
    {
        curSkill = attackSkill[randomNum - 1];
        damage = curSkill->calculateDamageEnemySkill(&battleStats, &target->curPersona->battleStats, &lv, &target->lv, &target->armour);
    }

    if (target->guarding)
    {
        iprintf("player guarded\n");
        damage *= 0.4;
    }

    bool attacked = false;
    if (curSkill->skillRace == SkillRace::mag)
    {
        if (DeductAttackCost(&sp, curSkill->cost, "not enough SP\n"))
        {
            attacked = true;
        }
    }
    else if (curSkill->skillRace == SkillRace::phys)
    {
        if (DeductAttackCost(&hp, curSkill->cost, "not enough HP\n"))
        {
            attacked = true;
        }
    }

    if (attacked)
    {
        u32 accuracy = curSkill->calculateHitrateEnemy(&battleStats, &target->curPersona->battleStats, &target->shoe);

        bool hitted = accuracy > u32(rand() % 100);

        if (!hitted)
        {
            iprintf("missed\n");
            return true;
        }
        target->hp -= (s32)damage;
        iprintf("Attack with: ");
        iprintf(curSkill->name.c_str());
        iprintf("\n");
        iprintf("Attacking: ");
        iprintf(target->name.c_str());
        iprintf("\n");
        char str[50];
        std::sprintf(str, "remaining target hp: %ld \n", target->hp);
        iprintf(str);
        /*
        if (player->hp <= 0)
        {
            player->hp = 72;
            counter = 0;
            exit();
            return;
        }
            */

        u32 affinity = target->curPersona->battleStats.affinities[curSkill->element];
        if (affinity == BattleStats::Affinity::Weak && !target->knockedDown && !target->guarding)
        {
            oneMore = true;
            iprintf("one more!\n");
            target->knockedDown = true;
        }
    }

    // just directly always ends for now
    return true;
}
#include "Enemy.h"
#include "../party/PartyMember.h"
#include <stdlib.h>

AttackSkill* Enemy::pickSkill()
{
    u32 roll = rand() % (attackCount + 1);
    return (roll == 0) ? baseAttackAction : attackSkill[roll - 1];
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

BattleResult Enemy::resolve(BattleParticipant* target, AttackSkill* skill)
{
    PartyMember* party = static_cast<PartyMember*>(target);

    s32* resource;
    if (skill->skillRace == SkillRace::mag)
        resource = &sp;
    else
        resource = &hp;

    bool canAfford = *resource > skill->cost;
    if (!canAfford)
        return {false, 0, false, skill->name};

    std::string targetLog = name + " targets " + target->name + "\n";

    *resource -= skill->cost;
    u32 accuracy = skill->calculateHitrateEnemy(&battleStats, &party->curPersona->battleStats, party->shoe);
    bool hit = accuracy > u32(rand() % 100);

    if (!hit)
        return {false, 0, false, targetLog + "Miss"};

    u32 damage = (skill == baseAttackAction)
                     ? skill->calculateDamageEnemyRegular(
                           &battleStats, &party->curPersona->battleStats, &lv, &target->lv, party->armour)
                     : skill->calculateDamageEnemySkill(
                           &battleStats, &party->curPersona->battleStats, &lv, &target->lv, party->armour);

    if (party->guarding)
        damage = (u32)(damage * 0.4f);

    bool oneMoreResult = false;
    u32 affinity = party->curPersona->battleStats.affinities[skill->element];
    if (affinity == BattleStats::Affinity::Weak && !party->knockedDown && !party->guarding)
    {
        oneMoreResult = true;
        party->knockedDown = true;
    }

    return {true, -(s32)damage, oneMoreResult, targetLog + skill->name};
}
