#include "BattleController.h"
#include "./battleActions/DeductAttackCost.h"
#include <cstdlib>
#include <ctime>

BattleController::BattleController(std::vector<BattleParticipant*>* iBattleParticipants, CharacterProfiles* iCharacterProfiles)
    : battleParticipants(iBattleParticipants)
    , characterProfiles(iCharacterProfiles)
{}

void BattleController::execute()
{
    active = true;

    player = new PartyMember(&characterProfiles->player);
    yukari = new PartyMember(&characterProfiles->yukari);
    junpei = new PartyMember(&characterProfiles->junpei);

    battleParticipants->push_back(player);
    battleParticipants->push_back(yukari);
    battleParticipants->push_back(junpei);

    for (BattleParticipant* p : *battleParticipants) {
        if (p->participantType == ParticipantType::Enemy)
            enemies.push_back(p);
        else
            partyMembers.push_back(p);
    }

    currentParticipantTurn = battleParticipants->at(0);
    if (currentParticipantTurn->participantType == ParticipantType::Enemy)
        phase = BattlePhase::EnemyTurn;
    else {
        actionIndex = 0;
        phase = BattlePhase::ChooseAction;
    }
}

void BattleController::update(u32 keys)
{
    if (!active) return;

    switch (phase) {

    case BattlePhase::Idle:
        break;

    case BattlePhase::ChooseAction: {
        PartyMember* actor = static_cast<PartyMember*>(currentParticipantTurn);
        u32 count = (u32)actions.size();
        updateIndex.update(keys, actionIndex, count);

        if (keys & KEY_LEFT || keys & KEY_RIGHT) {
            iprintf("Action: ");
            iprintf(actions[actionIndex]->name.c_str());
            iprintf("\n");
        }

        if (keys & KEY_A && actorCanUse(actor, actionIndex)) {
            if (actionIndex == ACTION_ATTACK) {
                targetIndex = 0;
                phase = BattlePhase::ChooseTarget;
            } else if (actionIndex == ACTION_GUARD) {
                BattleResult r = guard.resolve(actor, nullptr);
                applyResult(r);
                advanceTurn();
            } else if (actionIndex == ACTION_PERSONA) {
                skillIndex = 0;
                phase = BattlePhase::ChooseSkill;
            } else if (actionIndex == ACTION_SWITCH) {
                personaIndex = 0;
                phase = BattlePhase::ChoosePersona;
            }
        }
        break;
    }

    case BattlePhase::ChooseSkill: {
        PartyMember* actor = static_cast<PartyMember*>(currentParticipantTurn);
        u32 skillCount = actor->curPersona->attackCount;
        updateIndex.update(keys, skillIndex, skillCount);

        if (keys & KEY_LEFT || keys & KEY_RIGHT) {
            iprintf("Skill: ");
            iprintf(actor->curPersona->skills[skillIndex]->name.c_str());
            iprintf("\n");
        }

        if (keys & KEY_A) {
            Skill* s = actor->curPersona->skills[skillIndex];
            bool canAfford = false;
            if (s->skillRace == SkillRace::mag)
                canAfford = DeductAttackCost(&actor->sp, s->cost, "not enough SP\n");
            else
                canAfford = DeductAttackCost(&actor->hp, s->cost, "not enough HP\n");

            if (canAfford) {
                selectedSkill = s;
                targetIndex   = 0;
                phase = BattlePhase::ChooseTarget;
            }
        }

        if (keys & KEY_B)
            phase = BattlePhase::ChooseAction;
        break;
    }

    case BattlePhase::ChoosePersona: {
        PartyMember* actor = static_cast<PartyMember*>(currentParticipantTurn);
        u32 personaCount = (u32)actor->personas.size();
        updateIndex.update(keys, personaIndex, personaCount);

        if (keys & KEY_LEFT || keys & KEY_RIGHT) {
            iprintf("Persona: ");
            iprintf(actor->personas[personaIndex]->name.c_str());
            iprintf("\n");
        }

        if (keys & KEY_A) {
            if (actor->curPersona == actor->personas[personaIndex]) {
                iprintf("already selected this Persona\n");
            } else {
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

    case BattlePhase::ChooseTarget: {
        PartyMember* actor = static_cast<PartyMember*>(currentParticipantTurn);

        bool healTarget = (actionIndex == ACTION_PERSONA)
                       && selectedSkill
                       && (selectedSkill->skillType == SkillType::Heal);
        std::vector<BattleParticipant*>& targets = healTarget ? partyMembers : enemies;

        u32 count = (u32)targets.size();
        updateIndex.update(keys, targetIndex, count);

        if (keys & KEY_LEFT || keys & KEY_RIGHT) {
            iprintf("Target: ");
            iprintf(targets[targetIndex]->name.c_str());
            iprintf("\n");
        }

        if (keys & KEY_A) {
            BattleParticipant* target = targets[targetIndex];
            BattleResult r;
            if (actionIndex == ACTION_ATTACK)
                r = attack.resolve(actor, target);
            else
                r = persona.resolve(actor, target, selectedSkill);

            applyResult(r, target);
            advanceTurn();
        }

        if (keys & KEY_B) {
            targetIndex = 0;
            phase = (actionIndex == ACTION_ATTACK)
                    ? BattlePhase::ChooseAction
                    : BattlePhase::ChooseSkill;
        }
        break;
    }

    case BattlePhase::EnemyTurn: {
        Enemy* e = static_cast<Enemy*>(currentParticipantTurn);
        AttackSkill*       skill  = e->pickSkill();
        BattleParticipant* target = e->pickTarget(partyMembers);
        BattleResult r = e->resolve(target, skill);
        applyResult(r, target);
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
    phase  = BattlePhase::Done;
}

bool BattleController::actorCanUse(PartyMember* actor, u32 idx)
{
    if (actor->participantType == ParticipantType::Player)
        return true;
    return actions[idx]->possibleUsers == ParticipantType::Party;
}

void BattleController::applyResult(const BattleResult& r, BattleParticipant* target)
{
    if (!r.log.empty()) {
        iprintf(r.log.c_str());
        iprintf("\n");
    }

    if (r.hit && target && r.hpDelta != 0) {
        char buf[48];
        if (r.hpDelta < 0)
            std::sprintf(buf, "Damage: %ld\n", (long)-r.hpDelta);
        else
            std::sprintf(buf, "HP healed: %ld\n", (long)r.hpDelta);
        iprintf(buf);

        std::sprintf(buf, "%s HP: %ld\n", target->name.c_str(), (long)target->hp);
        iprintf(buf);
    }

    if (r.oneMore) {
        iprintf("One More!\n");
        currentParticipantTurn->oneMore = true;
    }
}

void BattleController::advanceTurn()
{
    removeDeadParticipants();
    if (!active) return;

    selectedSkill = nullptr;

    if (!currentParticipantTurn->oneMore) {
        turnsTaken++;
    } else {
        currentParticipantTurn->oneMore = false;
    }

    currentParticipantTurn = battleParticipants->at(turnsTaken % battleParticipants->size());
    if (currentParticipantTurn->participantType == ParticipantType::Enemy) {
        phase = BattlePhase::EnemyTurn;
    } else {
        actionIndex = 0;
        phase = BattlePhase::ChooseAction;
    }
}

void BattleController::removeDeadParticipants()
{
    for (u32 i = 0; i < battleParticipants->size(); ) {
        if (battleParticipants->at(i)->hp > 0) {
            i++;
            continue;
        }

        turnsTaken--;
        BattleParticipant* dead = battleParticipants->at(i);

        if (dead->participantType == ParticipantType::Player) {
            exit();
            return;
        } else if (dead->participantType == ParticipantType::Enemy) {
            for (u32 j = 0; j < enemies.size(); j++) {
                if (enemies[j] == dead) { enemies.erase(enemies.begin() + j); break; }
            }
            if (enemies.empty()) { exit(); return; }
        } else if (dead->participantType == ParticipantType::Party) {
            for (u32 j = 0; j < partyMembers.size(); j++) {
                if (partyMembers[j] == dead) { partyMembers.erase(partyMembers.begin() + j); break; }
            }
            if (partyMembers.empty()) { exit(); return; }
        }

        battleParticipants->erase(battleParticipants->begin() + i);
    }

    iprintf("Previous attacker: ");
    iprintf(currentParticipantTurn->name.c_str());
    iprintf("\n");
}
