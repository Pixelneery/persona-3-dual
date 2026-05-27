#include "BattleController.h"
#include "./battleActions/DeductAttackCost.h"
#include <cstdlib>
#include <ctime>
#include "core/globals.h"

BattleController::BattleController(std::vector<BattleParticipant *> *iBattleParticipants, CharacterProfiles *iCharacterProfiles)
    : battleParticipants(iBattleParticipants), characterProfiles(iCharacterProfiles)
{
}

void BattleController::execute()
{
    active = true;

    player = new PartyMember(&characterProfiles->player);
    yukari = new PartyMember(&characterProfiles->yukari);
    junpei = new PartyMember(&characterProfiles->junpei);

    battleParticipants->push_back(player);
    battleParticipants->push_back(yukari);
    battleParticipants->push_back(junpei);

    for (BattleParticipant *p : *battleParticipants)
    {
        if (p->participantType == ParticipantType::Enemy)
            enemies.push_back(p);
        else
            partyMembers.push_back(p);
    }

    currentParticipantTurn = battleParticipants->at(0);
    if (currentParticipantTurn->participantType == ParticipantType::Enemy)
        phase = BattlePhase::EnemyTurn;
    else
    {
        actionIndex = 0;
        phase = BattlePhase::ChooseAction;
    }
}

void BattleController::update(u32 keys)
{
    if (!active)
        return;

    switch (phase)
    {

    case BattlePhase::ChooseAction:
    {
        PartyMember *actor = static_cast<PartyMember *>(currentParticipantTurn);

        // render battleMenu
        battleMenuCmpt.loadActionOptions(&actions);
        actionIndex = -1;
        actionIndex = (int)battleMenuCmpt.update(keys);

        if (((int)actionIndex != -1) && (keys & KEY_A) && actorCanUse(actor, actionIndex))
        {
            consoleClear();
            if (actionIndex == ACTION_ATTACK)
            {
                selectedSkill = nullptr;
                targetIndex = 0;
                phase = BattlePhase::ChooseTarget;
            }
            else if (actionIndex == ACTION_GUARD)
            {
                BattleResult battleResult = guard.resolve(actor, nullptr);
                applyResult(battleResult);
                advanceTurn();
            }
            else if (actionIndex == ACTION_PERSONA)
            {
                skillIndex = 0;
                phase = BattlePhase::ChooseSkill;
            }
            else if (actionIndex == ACTION_SWITCH)
            {
                personaIndex = 0;
                phase = BattlePhase::ChoosePersona;
            }
        }
        break;
    }

    case BattlePhase::ChooseSkill:
    {
        PartyMember *actor = static_cast<PartyMember *>(currentParticipantTurn);
        u32 skillCount = actor->curPersona->attackCount;
        updateIndex.update(keys, skillIndex, skillCount);

        if (keys & KEY_LEFT || keys & KEY_RIGHT)
        {
            iprintf("Skill: ");
            iprintf(actor->curPersona->skills[skillIndex]->name.c_str());
            iprintf("\n");
        }

        if (keys & KEY_A)
        {
            Skill *s = actor->curPersona->skills[skillIndex];
            bool canAfford = false;
            if (s->skillRace == SkillRace::mag)
                canAfford = DeductAttackCost(&actor->sp, s->cost, "not enough SP\n");
            else
                canAfford = DeductAttackCost(&actor->hp, s->cost, "not enough HP\n");

            if (canAfford)
            {
                selectedSkill = s;
                targetIndex = 0;
                phase = BattlePhase::ChooseTarget;
            }
        }

        if (keys & KEY_B)
            phase = BattlePhase::ChooseAction;
        break;
    }

    case BattlePhase::ChoosePersona:
    {
        PartyMember *actor = static_cast<PartyMember *>(currentParticipantTurn);
        u32 personaCount = (u32)actor->personas.size();
        updateIndex.update(keys, personaIndex, personaCount);

        if (keys & KEY_LEFT || keys & KEY_RIGHT)
        {
            iprintf("Persona: ");
            iprintf(actor->personas[personaIndex]->name.c_str());
            iprintf("\n");
        }

        if (keys & KEY_A)
        {
            if (actor->curPersona == actor->personas[personaIndex])
            {
                iprintf("already selected this Persona\n");
            }
            else
            {
                actor->curPersona = actor->personas[personaIndex];
                iprintf("Switched to: ");
                iprintf(actor->curPersona->name.c_str());
                iprintf("\n");
                advanceTurn();
            }
        }

        if (keys & KEY_B)
            phase = BattlePhase::ChooseAction;
        break;
    }

    case BattlePhase::ChooseTarget:
    {
        PartyMember *actor = static_cast<PartyMember *>(currentParticipantTurn);

        bool healTarget = selectedSkill && selectedSkill->skillType == SkillType::Heal;
        std::vector<BattleParticipant *> &targets = healTarget ? partyMembers : enemies;

        u32 count = (u32)targets.size();
        updateIndex.update(keys, targetIndex, count);

        if (keys & KEY_LEFT || keys & KEY_RIGHT)
        {
            iprintf("Target: ");
            iprintf(targets[targetIndex]->name.c_str());
            iprintf("\n");
        }

        if (keys & KEY_A)
        {
            BattleParticipant *target = targets[targetIndex];
            BattleResult battleResult;
            if (actionIndex == ACTION_ATTACK)
                battleResult = attack.resolve(actor, target);
            else
                battleResult = persona.resolve(actor, target, selectedSkill);

            applyResult(battleResult, target);
            advanceTurn();
        }

        if (keys & KEY_B)
        {
            targetIndex = 0;
            phase = (actionIndex == ACTION_ATTACK) ? BattlePhase::ChooseAction : BattlePhase::ChooseSkill;
        }
        break;
    }

    case BattlePhase::EnemyTurn:
    {
        Enemy *enemy = static_cast<Enemy *>(currentParticipantTurn);
        AttackSkill *skill = enemy->pickSkill();
        BattleParticipant *target = enemy->pickTarget(partyMembers);
        BattleResult battleResult = enemy->resolve(target, skill);
        applyResult(battleResult, target);
        advanceTurn();
        break;
    }

    case BattlePhase::Done:
        break;
    }
}

void BattleController::exit()
{
    consoleClear();
    active = false;
    phase = BattlePhase::Done;
}

bool BattleController::actorCanUse(PartyMember *actor, u32 actionIndex)
{
    if (actor->participantType == ParticipantType::Player)
        return true;
    return actions[actionIndex]->possibleUsers == ParticipantType::Party;
}

void BattleController::applyResult(const BattleResult &battleResult, BattleParticipant *target)
{
    if (!battleResult.log.empty())
    {
        iprintf(battleResult.log.c_str());
        iprintf("\n");
    }

    if (battleResult.hit && target && battleResult.hpDelta != 0)
    {
        char buf[48];
        if (battleResult.hpDelta < 0)
            std::sprintf(buf, "Damage: %ld\n", (long)-battleResult.hpDelta);
        else
            std::sprintf(buf, "HP healed: %ld\n", (long)battleResult.hpDelta);
        iprintf(buf);

        std::sprintf(buf, "%s HP: %ld\n", target->name.c_str(), (long)target->hp);
        iprintf(buf);
    }

    if (battleResult.oneMore)
    {
        iprintf("One More!\n");
        currentParticipantTurn->oneMore = true;
    }
}

void BattleController::advanceTurn()
{
    removeDeadParticipants();
    if (!active)
        return;

    selectedSkill = nullptr;

    if (!currentParticipantTurn->oneMore)
    {
        turnsTaken++;
    }
    else
    {
        currentParticipantTurn->oneMore = false;
    }

    currentParticipantTurn = battleParticipants->at(turnsTaken % battleParticipants->size());
    if (currentParticipantTurn->participantType == ParticipantType::Enemy)
    {
        phase = BattlePhase::EnemyTurn;
    }
    else
    {
        actionIndex = 0;
        phase = BattlePhase::ChooseAction;
    }
}

void BattleController::removeDeadParticipants()
{
    for (u32 i = 0; i < battleParticipants->size(); i++)
    {
        if (battleParticipants->at(i)->hp > 0)
        {
            continue;
        }

        turnsTaken--;
        BattleParticipant *dead = battleParticipants->at(i);

        if (dead->participantType == ParticipantType::Player)
        {
            exit();
            return;
        }
        else if (dead->participantType == ParticipantType::Enemy)
        {
            for (u32 j = 0; j < enemies.size(); j++)
            {
                if (enemies[j] == dead)
                {
                    enemies.erase(enemies.begin() + j);
                    break;
                }
            }
            if (enemies.empty())
            {
                exit();
                return;
            }
        }
        else if (dead->participantType == ParticipantType::Party)
        {
            for (u32 j = 0; j < partyMembers.size(); j++)
            {
                if (partyMembers[j] == dead)
                {
                    partyMembers.erase(partyMembers.begin() + j);
                    break;
                }
            }
        }

        battleParticipants->erase(battleParticipants->begin() + i);
    }

    iprintf("Previous attacker: ");
    iprintf(currentParticipantTurn->name.c_str());
    iprintf("\n");
}
