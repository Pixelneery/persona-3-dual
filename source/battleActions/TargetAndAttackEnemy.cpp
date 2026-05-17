#include "TargetAndAttackEnemy.h"

bool TargetAndAttackActionEnemy::update(u32 *keys, AttackSkill *attack, PartyMember *user)
{

    if (*keys & KEY_LEFT || *keys & KEY_RIGHT)
    {
        iprintf("CurTar: ");
        iprintf(targets->at(*targetIndex)->name.c_str());
        iprintf("\n");
    }

    if (*keys & KEY_A)
    {
        iprintf("Attacking: ");
        iprintf(targets->at(*targetIndex)->name.c_str());
        iprintf("\n");

        u32 damage = attack->calculateDamagePlayer(user->curPersona->getBattleStats(), targets->at(*targetIndex)->battleStats.getBattleStats(), &user->lv, &targets->at(*targetIndex)->lv);

        u32 affinity = targets->at(*targetIndex)->battleStats.getBattleStats()->affinities[attack->element];
        if (affinity == BattleStats::Affinity::Weak && !targets->at(*targetIndex)->knockedDown)
        {
            user->oneMore = true;
            iprintf("one more!\n");
            targets->at(*targetIndex)->knockedDown = true;
        }

        targets->at(*targetIndex)->hp -= (s32)damage;

        char str1[25];
        std::sprintf(str1, "Damage: %ld \n", damage);
        iprintf(str1);

        char str2[50];
        std::sprintf(str2, "remaing Enemy hp: %ld \n", targets->at(*targetIndex)->hp);
        iprintf(str2);

        return true;
    }

    return false;
}
