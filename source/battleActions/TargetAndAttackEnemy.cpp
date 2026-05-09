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

        enemies->at(*targetIndex)->hp -= attack->calculateDamage(&player->curPersona->ma, &player->curPersona->st, &enemies->at(*targetIndex)->en, &player->lv, &enemies->at(*targetIndex)->lv);

        char str[50];
        std::sprintf(str, "remaing Enemy hp: %lu \n", enemies->at(*targetIndex)->hp);
        iprintf(str);
        iprintf("\n");

        return true;
    }

    return false;
}
