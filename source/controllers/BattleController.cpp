#include "BattleController.h"
#include "./helpers/random.h"
#include "core/globals.h"
#include <cstdlib>
#include <ctime>

BattleController::BattleController(std::vector<BattleParticipant*>* iBattleParticipants,
                                   CharacterProfiles* iCharacterProfiles,
                                   BattleStartCondition iBattleStartCondition)
    : battleParticipants(iBattleParticipants), characterProfiles(iCharacterProfiles),
      battleStartCondition(iBattleStartCondition)
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

    for (BattleParticipant* p : *battleParticipants)
    {
        if (p->participantType == ParticipantType::Enemy)
            enemies.push_back(p);
        else
            partyMembers.push_back(p);
    }

    calculateTurnOrder();

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
        PartyMember* actor = static_cast<PartyMember*>(currentParticipantTurn);

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
        PartyMember* actor = static_cast<PartyMember*>(currentParticipantTurn);

        // render battleMenu
        battleMenuCmpt.loadSkillOptions(actor->curPersona);
        skillIndex = -1;
        skillIndex = (int)battleMenuCmpt.update(keys);

        if (((int)skillIndex != -1) && (keys & KEY_A))
        {
            Skill* s = actor->curPersona->skills[skillIndex];

            s32* resource;
            if (s->skillRace == SkillRace::mag)
                resource = &actor->sp;
            else
                resource = &actor->hp;

            bool canAfford = *resource >= s->cost;
            if (canAfford)
            {
                *resource -= s->cost;
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
        PartyMember* actor = static_cast<PartyMember*>(currentParticipantTurn);

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
        PartyMember* actor = static_cast<PartyMember*>(currentParticipantTurn);

        bool healTarget = selectedSkill && selectedSkill->skillType == SkillType::Heal;

        std::vector<BattleParticipant*> targets;
        if (healTarget)
            targets = partyMembers;
        else
            targets = enemies;

        targets.erase(std::remove_if(targets.begin(), targets.end(), [](BattleParticipant* t) { return t->hp <= 0; }),
                      targets.end());

        battleMenuCmpt.loadTargetOptions(&targets, healTarget);
        targetIndex = -1;
        targetIndex = (int)battleMenuCmpt.update(keys);

        if ((int)targetIndex != -1 && (keys & KEY_A))
        {
            if ((selectedSkill->skillType == SkillType::Attack || selectedSkill->skillType == SkillType::Heal ||
                 selectedSkill->skillType == SkillType::Buff || selectedSkill->skillType == SkillType::Debuff))
            {
                targets = {targets.at(targetIndex)};
            }
            BattleParticipant* target = targets[targetIndex];
            // Check so you cant heal target that has max hp
            if (selectedSkill && selectedSkill->skillType == SkillType::Heal && target->hp >= target->maxHp)
                return;
            BattleResult battleResult = (actionIndex == ACTION_ATTACK) ? attack.resolve(actor, target)
                                                                       : persona.resolve(actor, target, selectedSkill);
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
        Enemy* enemy = static_cast<Enemy*>(currentParticipantTurn);
        AttackSkill* skill = enemy->pickSkill();
        BattleParticipant* target = enemy->pickTarget(partyMembers);
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

bool BattleController::actorCanUse(PartyMember* actor, u32 actionIndex)
{
    if (actor->participantType == ParticipantType::Player)
        return true;
    return actions[actionIndex]->possibleUsers == ParticipantType::Party;
}

void BattleController::applyResult(const BattleResult& battleResult, BattleParticipant* target)
{
    if (!battleResult.log.empty())
        pendingAlert += battleResult.log + "\n";

    if (battleResult.hit && target && battleResult.hpDelta != 0)
    {
        s32 hpBefore = target->hp;
        s32 actuallyHealedHp = battleResult.hpDelta;
        target->hp += battleResult.hpDelta;

        if (target->hp > target->maxHp)
        {
            target->hp = target->maxHp;
            actuallyHealedHp = target->maxHp - hpBefore;
        }

        char buf[48];
        if (battleResult.hpDelta < 0)
            std::sprintf(buf, "Damage: %ld\n", (long)-battleResult.hpDelta);
        else
            std::sprintf(buf, "HP healed: %ld\n", (long)actuallyHealedHp);
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
    handleDeadParticipants();
    if (!active)
        return;

    selectedSkill = nullptr;

    if (currentParticipantTurn->oneMore)
    {
        currentParticipantTurn->oneMore = false;
        BattlePhase nextPhase = (currentParticipantTurn->participantType == ParticipantType::Enemy)
                                    ? BattlePhase::EnemyTurn
                                    : BattlePhase::ChooseAction;

        setNextPhase(nextPhase);
        return;
    }

    currentParticipantTurn->knockedDown = false;

    u32 next = (currentParticipantIndex + 1) % battleParticipants->size();
    while (battleParticipants->at(next)->hp <= 0)
        next = (next + 1) % battleParticipants->size();

    currentParticipantIndex = next;
    turnsTaken++;
    currentParticipantTurn = battleParticipants->at(next);
    BattlePhase nextPhase;
    if (currentParticipantTurn->participantType == ParticipantType::Enemy)
    {
        actionIndex = -1;
        nextPhase = BattlePhase::EnemyTurn;
    }
    else
    {
        actionIndex = -1;
        nextPhase = BattlePhase::ChooseAction;
    }

    setNextPhase(nextPhase);
}

void BattleController::setNextPhase(BattlePhase nextPhase)
{
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

// TODO:: potentially rework since this would do a double turn if called again after 1st turn
void BattleController::calculateTurnOrder()
{
    // random boost from 1.2 to 1.4 that priorizes party
    float boost = 1.2f + (randf() * 0.2f);

    for (BattleParticipant* battleParticipant : *battleParticipants)
    {
        battleParticipant->currentTurnOrderAgility =
            battleParticipant->participantType == ParticipantType::Enemy
                ? static_cast<Enemy*>(battleParticipant)->battleStats.ag
                : static_cast<PartyMember*>(battleParticipant)->curPersona->battleStats.ag * boost;
    }

    if (battleStartCondition == BattleStartCondition::PartyAdvantage)
    {
        std::sort(battleParticipants->begin(),
                  battleParticipants->end(),
                  [](BattleParticipant* a, BattleParticipant* b)
                  {
                      bool aIsParty =
                          a->participantType == ParticipantType::Party || a->participantType == ParticipantType::Player;
                      bool bIsParty =
                          b->participantType == ParticipantType::Party || b->participantType == ParticipantType::Player;

                      //prioritize by type
                      if (aIsParty != bIsParty)
                          return aIsParty > bIsParty;
                      //decided by agility when same group
                      return a->currentTurnOrderAgility > b->currentTurnOrderAgility;
                  });
    }
    else if (battleStartCondition == BattleStartCondition::EnemyAdvantage)
    {
        std::sort(battleParticipants->begin(),
                  battleParticipants->end(),
                  [](BattleParticipant* a, BattleParticipant* b)
                  {
                      bool aIsEnemy = a->participantType == ParticipantType::Enemy;
                      bool bIsEnemy = b->participantType == ParticipantType::Enemy;

                      //prioritize by type
                      if (aIsEnemy != bIsEnemy)
                          return aIsEnemy > bIsEnemy;
                      //decided by agility when same group
                      return a->currentTurnOrderAgility > b->currentTurnOrderAgility;
                  });
    }
    else
    {
        std::sort(battleParticipants->begin(),
                  battleParticipants->end(),
                  [](BattleParticipant* a, BattleParticipant* b)
                  { return a->currentTurnOrderAgility > b->currentTurnOrderAgility; });
    }
}

void BattleController::handleDeadParticipants()
{
    for (u32 i = 0; i < battleParticipants->size(); i++)
    {
        if (battleParticipants->at(i)->hp > 0)
        {
            continue;
        }

        BattleParticipant* dead = battleParticipants->at(i);

        if (dead->participantType == ParticipantType::Player)
        {
            exit();
            return;
        }

        bool enemiesAlive = false;
        for (BattleParticipant* enemy : enemies)
        {
            if (enemy->hp > 0)
            {
                enemiesAlive = true;
                break;
            }
        }

        if (!enemiesAlive)
        {
            exit();
        }
    }

    pendingAlert += "Previous attacker: " + currentParticipantTurn->name + "\n";
}
