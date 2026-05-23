#include "TargetAndExecute.h"

bool TargetAndExecute::update(u32 *keys, Skill *skill, PartyMember *user, std::vector<BattleParticipant *> *targets)
{

    if (*keys & KEY_LEFT || *keys & KEY_RIGHT)
    {
        iprintf("CurTar: ");
        iprintf(targets->at(*targetIndex)->name.c_str());
        iprintf("\n");
    }

    if (*keys & KEY_A)
    {
        iprintf("Selected: ");
        iprintf(targets->at(*targetIndex)->name.c_str());
        iprintf("\n");

        if (skill->skillType == SkillType::Attack)
        {
            AttackSkill &attackSkill = static_cast<AttackSkill &>(*skill);
            Enemy &target = static_cast<Enemy &>(*targets->at(*targetIndex));

            u32 damage = attackSkill.calculateDamagePlayer(&user->curPersona->battleStats, &target.battleStats, &user->lv, &targets->at(*targetIndex)->lv);
            u32 accuracy = attackSkill.calculateHitratePlayer(&user->curPersona->battleStats, &target.battleStats);

            bool hitted = accuracy > u32(rand() % 100);

            if (!hitted)
            {
                iprintf("missed\n");
                return true;
            }

            u32 affinity = target.battleStats.affinities[attackSkill.element];
            if (affinity == BattleStats::Affinity::Weak && !target.knockedDown)
            {
                user->oneMore = true;
                iprintf("one more!\n");
                target.knockedDown = true;
            }

            target.hp -= (s32)damage;

            char str1[25];
            std::sprintf(str1, "Damage: %ld \n", damage);
            iprintf(str1);

            char str2[50];
            std::sprintf(str2, "remaing Enemy hp: %ld \n", target.hp);
            iprintf(str2);
        }
        else if (skill->skillType == SkillType::Heal)
        {
            HealSkill *healSkill = static_cast<HealSkill *>(skill);
            u32 hpHealed = healSkill->calculateHealing(*user);

            targets->at(*targetIndex)->hp += hpHealed;
            char str1[25];
            std::sprintf(str1, "Hp Healed: %ld \n", hpHealed);
            iprintf(str1);

            char str2[50];
            std::sprintf(str2, "New hp: %ld \n", targets->at(*targetIndex)->hp);
            iprintf(str2);
        }

        return true;
    }

    return false;
}
