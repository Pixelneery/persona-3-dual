#include "TargetAndAttackEnemy.h"

bool TargetAndAttackActionEnemy::update(u32 *keys, AttackSkill *attack)
{
    if (*keys & KEY_LEFT)
    {
        iprintf("CurTar: ");
        iprintf(enemies->at(*targetIndex)->name.c_str());
        iprintf("\n");
    }
    else if (*keys & KEY_RIGHT)
    {
        iprintf("CurTar: ");
        iprintf(enemies->at(*targetIndex)->name.c_str());
        iprintf("\n");
    }

    if (*keys & KEY_A)
    {
        iprintf("Attacking: ");
        iprintf(enemies->at(*targetIndex)->name.c_str());
        iprintf("\n");

        u32 damage = attack->calculateDamagePlayer(player->curPersona->getBattleStats(), enemies->at(*targetIndex)->getBattleStats(), &player->lv, &enemies->at(*targetIndex)->lv);

        u32 affinity = enemies->at(*targetIndex)->affinities[attack->element];
        if (affinity == BattleStats::Affinity::Weak && !enemies->at(*targetIndex)->knockedDown)
        {
            player->oneMore = true;
            iprintf("one more!\n");
            enemies->at(*targetIndex)->knockedDown = true;
        }

        enemies->at(*targetIndex)->hp -= (s32)damage;

        char str1[25];
        std::sprintf(str1, "Damage: %ld \n", damage);
        iprintf(str1);

        char str2[50];
        std::sprintf(str2, "remaing Enemy hp: %ld \n", enemies->at(*targetIndex)->hp);
        iprintf(str2);

        return true;
    }

    return false;
}
