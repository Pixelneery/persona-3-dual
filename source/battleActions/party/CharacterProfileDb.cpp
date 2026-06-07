#include "CharacterProfileDb.h"
#include "../armours/ArmourDb.h"
#include "../personas/PersonaDb.h"
#include "../shoes/ShoeDb.h"
#include "../skills/SkillDb.h"

CharacterProfile CharacterProfiles::player;
CharacterProfile CharacterProfiles::yukari;
CharacterProfile CharacterProfiles::junpei;

void CharacterProfiles::Initialize()
{
    /*--------------MC-----------------*/
    player.name = "Player";
    player.maxHp = 72;
    player.hp = 72;
    player.maxSp = 50;
    player.sp = 50;
    player.lv = 2;
    player.participantType = ParticipantType::Player;
    player.armourType = ArmourType::Male;
    player.baseAttackAction = &SkillDb::slashAttack;
    player.armour = ArmourDb::plainShirt;
    player.shoe = ShoeDb::rubberSole;
    player.personas.push_back(&PersonaDb::orpheus);
    player.personas.push_back(&PersonaDb::forneus);
    player.curPersona = player.personas.at(0);

    /*--------------YUKARI-------------*/
    yukari.name = "Yukari";
    yukari.maxHp = 51;
    yukari.hp = 51;
    yukari.maxSp = 45;
    yukari.sp = 45;
    yukari.lv = 1;
    yukari.participantType = ParticipantType::Party;
    yukari.armourType = ArmourType::Female;
    yukari.baseAttackAction = &SkillDb::pierceAttack;
    yukari.armour = ArmourDb::rashGuard;
    yukari.shoe = ShoeDb::rubberSole;
    yukari.personas.push_back(&PersonaDb::io);
    yukari.curPersona = yukari.personas.at(0);

    /*--------------JUNPEI-------------*/
    junpei.name = "Junpei";
    junpei.maxHp = 70;
    junpei.hp = 70;
    junpei.maxSp = 36;
    junpei.sp = 36;
    junpei.lv = 1;
    junpei.participantType = ParticipantType::Party;
    junpei.armourType = ArmourType::Male;
    junpei.baseAttackAction = &SkillDb::slashAttack;
    junpei.armour = ArmourDb::rashGuard;
    junpei.shoe = ShoeDb::rubberSole;
    junpei.personas.push_back(&PersonaDb::hermes);
    junpei.curPersona = junpei.personas.at(0);
}
