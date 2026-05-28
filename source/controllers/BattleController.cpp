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
        actionIndex = -1;
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
        battleMenuCmpt.loadActionOptions(&actions, actor->name);
        actionIndex = -1;
        actionIndex = (int)battleMenuCmpt.update(keys);

        if (((int)actionIndex != -1) && (keys & KEY_A) && actorCanUse(actor, actionIndex))
        {
            consoleClear();
            if (actionIndex == ACTION_ATTACK)
            {
                selectedSkill = nullptr;
                targetIndex = -1;
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
                skillIndex = -1;
                phase = BattlePhase::ChooseSkill;
            }
            else if (actionIndex == ACTION_SWITCH)
            {
                personaIndex = -1;
                phase = BattlePhase::ChoosePersona;
            }
        }
        break;
    }

    case BattlePhase::ChooseSkill:
    {
        PartyMember *actor = static_cast<PartyMember *>(currentParticipantTurn);

        // render battleMenu
        battleMenuCmpt.loadSkillOptions(actor->curPersona);
        skillIndex = -1;
        skillIndex = (int)battleMenuCmpt.update(keys);

        if (((int)skillIndex != -1) && (keys & KEY_A))
        {
            Skill *s = actor->curPersona->skills[skillIndex];
            bool canAfford = false;
            if (s->skillRace == SkillRace::mag)
                canAfford = DeductAttackCost(&actor->sp, s->cost);
            else
                canAfford = DeductAttackCost(&actor->hp, s->cost);

            if (canAfford)
            {
                selectedSkill = s;
                targetIndex = 0;
                phase = BattlePhase::ChooseTarget;
            }
            else
            {
                pendingAlert = (s->skillRace == SkillRace::mag) ? "Not enough SP\n" : "Not enough HP\n";
                alertReturnPhase = BattlePhase::ChooseSkill;
                battleMenuCmpt.reset();
                phase = BattlePhase::ShowAlert;
            }
        }

        if (keys & KEY_B)
            phase = BattlePhase::ChooseAction;
        break;
    }

    case BattlePhase::ChoosePersona:
    {
        PartyMember *actor = static_cast<PartyMember *>(currentParticipantTurn);

        battleMenuCmpt.loadPersonaOptions(&actor->personas);
        personaIndex = -1;
        personaIndex = (int)battleMenuCmpt.update(keys);

        if (((int)personaIndex != -1) && (keys & KEY_A))
        {
            if (actor->curPersona == actor->personas[personaIndex])
            {
                pendingAlert = "Already using this Persona\n";
                alertReturnPhase = BattlePhase::ChoosePersona;
                battleMenuCmpt.reset();
                phase = BattlePhase::ShowAlert;
            }
            else
            {
                actor->curPersona = actor->personas[personaIndex];
                pendingAlert = "Switched to: " + actor->curPersona->name + "\n";
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

        battleMenuCmpt.loadTargetOptions(&targets, healTarget);
        targetIndex = -1;
        targetIndex = (int)battleMenuCmpt.update(keys);

        if (((int)targetIndex != -1) && (keys & KEY_A))
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
            targetIndex = -1;
            phase = (actionIndex == ACTION_ATTACK) ? BattlePhase::ChooseAction : BattlePhase::ChooseSkill;
        }
        break;
    }

    case BattlePhase::ShowAlert:
    {
        battleMenuCmpt.loadAlertOptions(pendingAlert);
        battleMenuCmpt.update(keys);
        if (battleMenuCmpt.isAlertExpired(120))
        {
            pendingAlert.clear();
            battleMenuCmpt.reset();
            phase = alertReturnPhase;
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
        pendingAlert += battleResult.log + "\n";

    if (battleResult.hit && target && battleResult.hpDelta != 0)
    {
        char buf[48];
        if (battleResult.hpDelta < 0)
            std::sprintf(buf, "Damage: %ld\n", (long)-battleResult.hpDelta);
        else
            std::sprintf(buf, "HP healed: %ld\n", (long)battleResult.hpDelta);
        pendingAlert += buf;

        std::sprintf(buf, "%s HP: %ld\n", target->name.c_str(), (long)target->hp);
        pendingAlert += buf;
    }

    if (battleResult.oneMore)
    {
        pendingAlert += "One More!\n";
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

    BattlePhase nextPhase;
    if (currentParticipantTurn->participantType == ParticipantType::Enemy)
    {
        nextPhase = BattlePhase::EnemyTurn;
    }
    else
    {
        actionIndex = -1;
        nextPhase = BattlePhase::ChooseAction;
    }

    if (!pendingAlert.empty())
    {
        alertReturnPhase = nextPhase;
        battleMenuCmpt.reset();
        phase = BattlePhase::ShowAlert;
    }
    else
    {
        phase = nextPhase;
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

    pendingAlert += "Previous attacker: " + currentParticipantTurn->name + "\n";
}
