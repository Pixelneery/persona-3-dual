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

    std::string path =
        fatBasePath + "music/battle/" + (saveData.femcMode ? "wiping_all_out.pcm" : "mass_destruction.pcm");
    musicCtrl.init(path.c_str(), 0.0f, -1.0f);

    player = new Player(&characterProfiles->player);
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

    menuIndex = -1;
    phase = currentParticipantTurn->getInitalTurnPhase();
}

BattleResult BattleController::update(u32 keys)
{
    if (!active)
        return battleResult;

    switch (phase)
    {
    case BattlePhase::ChooseAction:
    {
        PartyMember* actor = static_cast<PartyMember*>(currentParticipantTurn);

        // render battleMenu
        battleMenuCmpt.loadActionOptions(&actions, actor->name);
        menuIndex = -1;
        menuIndex = (int)battleMenuCmpt.update(keys);

        if ((menuIndex != -1) && (keys & KEY_A) && actor->actorCanUse(actions[menuIndex]))
        {
            consoleClear();
            if (menuIndex == ACTION_ATTACK)
            {
                selectedSkill = actor->baseAttackAction;
                phase = BattlePhase::ChooseTarget;
            }
            else if (menuIndex == ACTION_GUARD)
            {
                TurnResult turnResult = guard.resolve(actor, nullptr);
                applyResult(turnResult);
                advanceTurn();
            }
            else if (menuIndex == ACTION_PERSONA)
            {
                phase = BattlePhase::ChooseSkill;
            }
            else if (menuIndex == ACTION_SWITCH)
            {
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
        //TODO: why not just return nullptr if nothing happens instead of setting -1 manually everywhere?

        menuIndex = -1;
        menuIndex = (int)battleMenuCmpt.update(keys);

        if ((menuIndex != -1) && (keys & KEY_A))
        {
            Skill* s = actor->curPersona->skills[menuIndex];

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
                menuIndex = 0;
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
        menuIndex = -1;
        menuIndex = (int)battleMenuCmpt.update(keys);

        if ((menuIndex != -1) && (keys & KEY_A))
        {
            if (actor->curPersona == actor->personas[menuIndex])
            {
                pendingAlert = "Already using this Persona\n";
                alertReturnPhase = BattlePhase::ChoosePersona;
                battleMenuCmpt.reset();
                phase = BattlePhase::ShowAlert;
            }
            else
            {
                actor->curPersona = actor->personas[menuIndex];
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

        bool healTarget = selectedSkill && (selectedSkill->skillType == SkillType::Heal ||
                                            selectedSkill->skillType == SkillType::MultiHeal);

        std::vector<BattleParticipant*> targets;
        if (healTarget)
            targets = partyMembers;
        else
            targets = enemies;

        targets.erase(std::remove_if(targets.begin(), targets.end(), [](BattleParticipant* t) { return t->hp <= 0; }),
                      targets.end());

        battleMenuCmpt.loadTargetOptions(&targets, healTarget);
        menuIndex = -1;
        menuIndex = (int)battleMenuCmpt.update(keys);

        if (menuIndex != -1 && (keys & KEY_A))
        {
            if (isSingleTarget(selectedSkill->skillType))
            {
                targets = {targets[menuIndex]};
            }

            // Check so you cant heal target that has max hp
            if (selectedSkill &&
                (selectedSkill->skillType == SkillType::Heal || selectedSkill->skillType == SkillType::MultiHeal))
            {
                bool canHealAnyTarget = false;
                for (BattleParticipant* target : targets)
                {
                    if (target->hp < target->maxHp)
                    {
                        canHealAnyTarget = true;
                        break;
                    }
                }
                if (!canHealAnyTarget)
                    return battleResult;
            }

            for (BattleParticipant* target : targets)
            {
                TurnResult turnResult = (menuIndex == ACTION_ATTACK) ? attack.resolve(actor, target)
                                                                     : persona.resolve(actor, target, selectedSkill);
                applyResult(turnResult, target);
            }

            advanceTurn();
        }

        if (keys & KEY_B)
        {
            phase = (selectedSkill == actor->baseAttackAction) ? BattlePhase::ChooseAction : BattlePhase::ChooseSkill;
            menuIndex = -1;
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
        Skill* skill = enemy->pickSkill();
        BattleParticipant* target = enemy->pickTarget(partyMembers);
        TurnResult turnResult = enemy->resolve(target, skill);
        applyResult(turnResult, target);
        advanceTurn();
        break;
    }

    case BattlePhase::Done:
        break;
    }

    return battleResult;
}

void BattleController::exit()
{
    consoleClear();
    musicCtrl.pause();
    active = false;
    phase = BattlePhase::Done;
}

void BattleController::applyResult(const TurnResult& turnResult, BattleParticipant* target)
{
    if (!turnResult.log.empty())
        pendingAlert += turnResult.log + "\n";

    if (turnResult.hit && target && turnResult.hpDelta != 0)
    {
        s32 hpBefore = target->hp;
        s32 actuallyHealedHp = turnResult.hpDelta;
        target->hp += turnResult.hpDelta;

        if (target->hp > target->maxHp)
        {
            target->hp = target->maxHp;
            actuallyHealedHp = target->maxHp - hpBefore;
        }

        char buf[48];
        if (turnResult.hpDelta < 0)
            std::sprintf(buf, "Damage: %ld\n", (long)-turnResult.hpDelta);
        else
            std::sprintf(buf, "HP healed: %ld\n", (long)actuallyHealedHp);
        pendingAlert += buf;

        std::sprintf(buf, "%s HP: %ld\n", target->name.c_str(), (long)target->hp);
        pendingAlert += buf;
    }

    if (turnResult.oneMore)
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

    pendingAlert += "Previous attacker: " + currentParticipantTurn->name + "\n";

    selectedSkill = nullptr;

    if (currentParticipantTurn->oneMore)
    {
        currentParticipantTurn->oneMore = false;
        BattlePhase nextPhase = currentParticipantTurn->getInitalTurnPhase();

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

    menuIndex = -1;
    BattlePhase nextPhase = currentParticipantTurn->getInitalTurnPhase();

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
        battleParticipant->setCurrentTurnOrderAgility(boost);
    }

    std::sort(partyMembers.begin(), partyMembers.end(), getParticipantByHigherAgility);
    std::sort(enemies.begin(), enemies.end(), getParticipantByHigherAgility);

    battleParticipants->clear();

    if (battleStartCondition == BattleStartCondition::PartyAdvantage)
    {
        for (auto p : partyMembers)
        {
            battleParticipants->push_back(p);
        }
        for (auto e : enemies)
        {
            battleParticipants->push_back(e);
        }
    }
    else if (battleStartCondition == BattleStartCondition::EnemyAdvantage)
    {
        for (auto e : enemies)
        {
            battleParticipants->push_back(e);
        }
        for (auto p : partyMembers)
        {
            battleParticipants->push_back(p);
        }
    }
    else
    {
        std::merge(partyMembers.begin(),
                   partyMembers.end(),
                   enemies.begin(),
                   enemies.end(),
                   std::back_inserter(*battleParticipants),
                   getParticipantByHigherAgility);
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

        dead->onDead(battleResult);

        if (battleResult.playerDied)
        {
            exit();
            return;
        }
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
        return;
    }
}

bool BattleController::isSingleTarget(SkillType type)
{
    switch (type)
    {
    case SkillType::RegularAttack:
    case SkillType::Attack:
    case SkillType::Heal:
    case SkillType::Buff:
    case SkillType::Debuff:
        return true;
    default:
        return false;
    }
}
