#include "BattleController.h"
#include "./battleActions/DeductAttackCost.h"
#include <cstdlib>
#include <ctime>
#include "./helpers/random.h"

BattleController::BattleController(std::vector<BattleParticipant *> *iBattleParticipants, CharacterProfiles *iCharacterProfiles, BattleStartCondition iBattleStartCondition)
    : battleParticipants(iBattleParticipants), characterProfiles(iCharacterProfiles), battleStartCondition(iBattleStartCondition) {}

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

    calculateTurnOrder();

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
        u32 count = (u32)actions.size();
        updateIndex.update(keys, actionIndex, count);

        if (keys & KEY_LEFT || keys & KEY_RIGHT)
        {
            iprintf("Action: ");
            iprintf(actions[actionIndex]->name.c_str());
            iprintf("\n");
        }

        if (keys & KEY_A && actorCanUse(actor, actionIndex))
        {
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

        if (keys & KEY_B)
            phase = BattlePhase::ChooseAction;

        else if (keys & KEY_A)
        {
            if (actor->curPersona == actor->personas[personaIndex])
            {
                iprintf("already selected this Persona\n");
                return;
            }

            actor->curPersona = actor->personas[personaIndex];
            iprintf("Switched to: ");
            iprintf(actor->curPersona->name.c_str());
            iprintf("\n");
            advanceTurn();
        }

        break;
    }

    case BattlePhase::ChooseTarget:
    {
        PartyMember *actor = static_cast<PartyMember *>(currentParticipantTurn);

        bool healTarget = selectedSkill && selectedSkill->skillType == SkillType::Heal;
        std::vector<BattleParticipant *> &targets = healTarget ? partyMembers : enemies;

        u32 count = (u32)targets.size();
        updateIndex.updateSkipDead(keys, targetIndex, targets);

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
    handleDeadParticipants();
    if (!active)
        return;

    selectedSkill = nullptr;

    if (currentParticipantTurn->oneMore)
    {
        currentParticipantTurn->oneMore = false;
        actionIndex = 0;
        return;
    }

    u32 next = (currentParticipantIndex + 1) % battleParticipants->size();
    while (battleParticipants->at(next)->hp <= 0)
        next = (next + 1) % battleParticipants->size();

    currentParticipantIndex = next;
    turnsTaken++;
    currentParticipantTurn = battleParticipants->at(next);

    if (currentParticipantTurn->participantType == ParticipantType::Enemy)
        phase = BattlePhase::EnemyTurn;
    else
        phase = BattlePhase::ChooseAction;
}

// TODO:: potentially rework since this would do a double turn if called again after 1st turn
void BattleController::calculateTurnOrder()
{
    // random boost from 1.2 to 1.4 that priorizes party
    float boost = 1.2f + (randf() * 0.2f);
    // TODO: check seeding
    /*
    iprintf("boost:%d\n", (int)(boost * 10));
    while (!(keysDown() & KEY_A))
    {
        scanKeys();
    }

    */

    for (BattleParticipant *battleParticipant : *battleParticipants)
    {
        battleParticipant->currentTurnOrderAgility = battleParticipant->participantType == ParticipantType::Enemy
                                                         ? static_cast<Enemy *>(battleParticipant)->battleStats.ag
                                                         : static_cast<PartyMember *>(battleParticipant)->curPersona->battleStats.ag * boost;
    }

    if (battleStartCondition == BattleStartCondition::PartyAdvantage)
    {
        std::sort(battleParticipants->begin(), battleParticipants->end(), [](BattleParticipant *a, BattleParticipant *b)
                  {
                          bool aIsParty = a->participantType == ParticipantType::Party || a->participantType == ParticipantType::Player;
                          bool bIsParty = b->participantType == ParticipantType::Party || b->participantType == ParticipantType::Player;

                        //priotetize by type
                          if (aIsParty != bIsParty)
                              return aIsParty > bIsParty;
                        //decided by agility when same group
                          return a->currentTurnOrderAgility > b->currentTurnOrderAgility; });
    }
    else if (battleStartCondition == BattleStartCondition::EnemyAdvantage)
    {
        std::sort(battleParticipants->begin(), battleParticipants->end(), [](BattleParticipant *a, BattleParticipant *b)
                  {
                          bool aIsEnemy = a->participantType == ParticipantType::Enemy;
                          bool bIsEnemy = b->participantType == ParticipantType::Enemy;

                        //priotetize by type
                          if (aIsEnemy != bIsEnemy)
                              return aIsEnemy > bIsEnemy;
                        //decided by agility when same group
                          return a->currentTurnOrderAgility > b->currentTurnOrderAgility; });
    }
    else
    {
        std::sort(battleParticipants->begin(), battleParticipants->end(), [](BattleParticipant *a, BattleParticipant *b)
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

        BattleParticipant *dead = battleParticipants->at(i);

        if (dead->participantType == ParticipantType::Player)
        {
            exit();
            return;
        }

        // Todo: check fro dead enemies

        if (enemies.empty())
        {
            exit();
        }
    }

    iprintf("Previous attacker: ");
    iprintf(currentParticipantTurn->name.c_str());
    iprintf("\n");
}
