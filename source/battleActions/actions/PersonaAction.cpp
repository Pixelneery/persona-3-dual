#include "PersonaAction.h"
#include <stdio.h>

void PersonaAction::execute()
{
    inProgress = true;
    targetIndex = 0;
    menuState = SelectSkill;
}

bool PersonaAction::update(u32 *keys, PartyMember *user)
{
    if (menuState == SelectSkill)
    {
        u32 skillCount = user->curPersona->attackCount;
        updateIndex.update(*keys, targetIndex, skillCount);

        Skill *curSkill = user->curPersona->skills[targetIndex];

        if (*keys & KEY_LEFT || *keys & KEY_RIGHT)
        {
            iprintf("Cur: ");
            iprintf(curSkill->name.c_str());
            iprintf("\n");
        }

        if (*keys & KEY_A)
        {
            if (curSkill->skillRace == SkillRace::mag)
            {
                if (!DeductAttackCost(&user->sp, curSkill->cost, "not enough SP\n"))
                    return false;
            }
            else if (curSkill->skillRace == SkillRace::phys)
            {
                if (!DeductAttackCost(&user->hp, curSkill->cost, "not enough HP\n"))
                    return false;
            }

            iprintf("Sel: ");
            selectedSkill = curSkill;
            iprintf(selectedSkill->name.c_str());
            iprintf("\n");

            targetIndex = 0;
            menuState = SelectTarget;
        }
    }
    else if (menuState == SelectTarget)
    {
        bool madeAction = false;

        if (selectedSkill->skillType == SkillType::Attack)
        {
            u32 enemyCount = enemies->size();
            updateIndex.update(*keys, targetIndex, enemyCount);
            madeAction = targetAndExecute->update(keys, selectedSkill, user, enemies);
        }
        else if (selectedSkill->skillType == SkillType::Heal)
        {
            u32 partyCount = party->size();
            updateIndex.update(*keys, targetIndex, partyCount);
            madeAction = targetAndExecute->update(keys, selectedSkill, user, party);
        }

        if (madeAction)
        {
            targetIndex = 0;
            inProgress = false;
            return true;
        }
    }

    if (*keys & KEY_B)
    {
        if (menuState == SelectSkill)
        {
            inProgress = false;
        }
        else if (menuState == SelectTarget)
        {
            targetIndex = 0;
            menuState = SelectSkill;
        }
    }

    return false;
}
#include "PersonaAction.h"
#include <stdlib.h>

BattleResult PersonaAction::resolve(PartyMember* user, BattleParticipant* target, Skill* skill)
{
    if (skill->skillType == SkillType::Attack)
    {
        AttackSkill* atk = static_cast<AttackSkill*>(skill);
        Enemy* enemy = static_cast<Enemy*>(target);

        u32 accuracy = atk->calculateHitratePlayer(&user->curPersona->battleStats, &enemy->battleStats);
        bool hit = accuracy > u32(rand() % 100);

        if (!hit)
            return {false, 0, false, "Miss"};

        u32 damage =
            atk->calculateDamagePlayer(&user->curPersona->battleStats, &enemy->battleStats, &user->lv, &target->lv);
        bool oneMore = false;

        u32 affinity = enemy->battleStats.affinities[atk->element];
        if (affinity == BattleStats::Affinity::Weak && !enemy->knockedDown)
        {
            oneMore = true;
            enemy->knockedDown = true;
        }

        return {true, -(s32)damage, oneMore, atk->name};
    }
    else if (skill->skillType == SkillType::Heal)
    {
        HealSkill* heal = static_cast<HealSkill*>(skill);
        u32 healed = heal->calculateHealing(*user);
        return {true, (s32)healed, false, skill->name};
    }

    return {false, 0, false, skill->name};
}
