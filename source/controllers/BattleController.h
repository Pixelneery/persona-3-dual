#pragma once

#include <nds.h>
#include <array>
#include <vector>

#include "./battleActions/actions/AttackAction.h"
#include "./battleActions/actions/Guard.h"
#include "./battleActions/actions/PersonaAction.h"
#include "./battleActions/actions/SwitchPersona.h"

#include "./battleActions/BattleParticipant.h"
#include "./battleActions/BattleResult.h"
#include "./battleActions/enemies/Enemy.h"
#include "./battleActions/party/PartyMember.h"
#include "./battleActions/party/CharacterProfiles.h"

enum class BattlePhase
{
    ChooseAction,
    ChooseSkill,
    ChoosePersona,
    ChooseTarget,
    EnemyTurn,
    Done
};

class BattleController
{
private:
    static constexpr u32 ACTION_ATTACK = 0;
    static constexpr u32 ACTION_GUARD = 1;
    static constexpr u32 ACTION_PERSONA = 2;
    static constexpr u32 ACTION_SWITCH = 3;

    bool active = false;
    u32 turnsTaken = 0;

    BattlePhase phase;
    BattleParticipant *currentParticipantTurn = nullptr;

    u32 actionIndex = 0;
    u32 skillIndex = 0;
    u32 personaIndex = 0;
    u32 targetIndex = 0;
    Skill *selectedSkill = nullptr;

    std::vector<BattleParticipant *> *battleParticipants;
    std::vector<BattleParticipant *> enemies;
    std::vector<BattleParticipant *> partyMembers;

    CharacterProfiles *characterProfiles;

    AttackAction attack;
    Guard guard;
    PersonaAction persona;
    SwitchPersona switchPersona;

    std::array<ActionBase *, 4> actions = {&attack, &guard, &persona, &switchPersona};

    PartyMember *player = nullptr;
    PartyMember *yukari = nullptr;
    PartyMember *junpei = nullptr;

    bool actorCanUse(PartyMember *actor, u32 idx);
    void applyResult(const BattleResult &r, BattleParticipant *target = nullptr);
    void advanceTurn();
    void removeDeadParticipants();

public:
    bool isActive() const { return active; }
    BattlePhase getPhase() const { return phase; }

    void execute();
    void update(u32 keys);
    void exit();

    BattleController(std::vector<BattleParticipant *> *iBattleParticipants, CharacterProfiles *iCharacterProfiles);
    ~BattleController() {}
};
