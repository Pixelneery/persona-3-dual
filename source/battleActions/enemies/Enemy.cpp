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
    if (curSkill->race == AttackSkill::mag)
    {
        if (DeductAttackCost(&sp, curSkill->cost, "not enough SP\n"))
        {
            attacked = true;
        }
    }
    else if (curSkill->race == AttackSkill::phys)
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
