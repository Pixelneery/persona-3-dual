#pragma once

#include <algorithm>
#include <array>
#include <nds.h>
#include <string>
#include <vector>

#include "./battleActions/actions/AttackAction.h"
#include "./battleActions/actions/Guard.h"
#include "./battleActions/actions/PersonaAction.h"
#include "./battleActions/actions/SwitchPersona.h"

#include "./battleActions/BattleParticipant.h"
#include "./battleActions/BattleResult.h"
#include "./battleActions/BattleStartCondition.h"
#include "./battleActions/enemies/Enemy.h"
#include "./battleActions/party/CharacterProfileDb.h"
#include "./battleActions/party/PartyMember.h"

// Battle music tracks
static const int GN_TRACK_COUNT   = 5;
static const int FEMC_TRACK_COUNT = 2;
static const int MC_TRACK_COUNT   = 1;

static const char* GN_TRACKS[GN_TRACK_COUNT] = {
    "music/battle/laser_beam.pcm",
    "music/battle/mass_destruction.pcm",
    "music/battle/burn_my_dread_last_battle.pcm",
    "music/battle/mass_destruction_reincarnation.pcm",
    "music/battle/light_the_fire_up_kagejikan.pcm",
};
static const char* FEMC_TRACKS[FEMC_TRACK_COUNT] = {
    "music/battle/danger_zone.pcm",
    "music/battle/wiping_all_out.pcm",
};
static const char* MC_TRACKS[MC_TRACK_COUNT] = {
    "music/battle/light_the_fire_up_mayonaka.pcm",
};

enum class BattlePhase
{
    ChooseAction,
    ChooseSkill,
    ChoosePersona,
    ChooseTarget,
    ShowAlert,
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
    BattleParticipant* currentParticipantTurn = nullptr;
    u32 currentParticipantIndex = 0;

    u32 actionIndex = 0;
    u32 skillIndex = 0;
    u32 personaIndex = 0;
    u32 targetIndex = 0;
    Skill* selectedSkill = nullptr;

    std::string pendingAlert;
    BattlePhase alertReturnPhase = BattlePhase::Done;

    std::vector<BattleParticipant*>* battleParticipants;
    std::vector<BattleParticipant*> enemies;
    std::vector<BattleParticipant*> partyMembers;

    CharacterProfiles* characterProfiles;
    BattleStartCondition battleStartCondition = BattleStartCondition::Even;

    AttackAction attack;
    Guard guard;
    PersonaAction persona;
    SwitchPersona switchPersona;

    std::array<ActionBase*, 4> actions = {&attack, &guard, &persona, &switchPersona};

    PartyMember* player = nullptr;
    PartyMember* yukari = nullptr;
    PartyMember* junpei = nullptr;

    bool actorCanUse(PartyMember* actor, u32 idx);
    void applyResult(const BattleResult& r, BattleParticipant* target = nullptr);
    void advanceTurn();
    void setNextPhase(BattlePhase nextPhase);
    void calculateTurnOrder();
    void handleDeadParticipants();
    bool isSingleTarget(SkillType type);

  public:
    bool isActive() const
    {
        return active;
    }
    BattlePhase getPhase() const
    {
        return phase;
    }

    void execute();
    void update(u32 keys);
    void exit();

    BattleController(std::vector<BattleParticipant*>* iBattleParticipants,
                     CharacterProfiles* iCharacterProfiles,
                     BattleStartCondition iBattleStartCondition);
    ~BattleController()
    {
    }
};
