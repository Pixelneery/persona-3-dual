#include "PersonaAction.h"
#include <stdlib.h>

BattleResult PersonaAction::resolve(PartyMember *user, BattleParticipant *target, Skill *skill)
{
    if (skill->skillType == SkillType::Attack)
    {
        AttackSkill *atk = static_cast<AttackSkill *>(skill);
        Enemy *enemy = static_cast<Enemy *>(target);

        u32 accuracy = atk->calculateHitratePlayer(&user->curPersona->battleStats, &enemy->battleStats);
        bool hit = accuracy > u32(rand() % 100);

        if (!hit)
            return {false, 0, false, "Miss"};

        u32 damage = atk->calculateDamagePlayer(&user->curPersona->battleStats, &enemy->battleStats, &user->lv, &target->lv);
        bool oneMore = false;

        u32 affinity = enemy->battleStats.affinities[atk->element];
        if (affinity == BattleStats::Affinity::Weak && !enemy->knockedDown)
        {
            oneMore = true;
            enemy->knockedDown = true;
        }

        target->hp -= (s32)damage;
        return {true, -(s32)damage, oneMore, atk->name};
    }
    else if (skill->skillType == SkillType::Heal)
    {
        HealSkill *heal = static_cast<HealSkill *>(skill);
        u32 healed = heal->calculateHealing(*user);
        target->hp += (s32)healed;
        return {true, (s32)healed, false, skill->name};
    }

    return {false, 0, false, skill->name};
}
