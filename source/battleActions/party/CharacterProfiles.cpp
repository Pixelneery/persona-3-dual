#include "CharacterProfiles.h"

void CharacterProfiles::InitializeProfiles()
{
    RashGuard rashGuard;
    RubberSole rubberSole;

    /*--------------MC-----------------*/

    player.name = "Player";
    player.maxHp = 72;
    player.hp = 72;
    player.maxSp = 50;
    player.sp = 50;
    player.lv = 2;

    player.baseAttackAction = new Slash_Attack;
    player.participantType = ParticipantType::Player;

    PlainShirt plainShirt;
    player.armourType = ArmourType::Male;
    player.armour = plainShirt;
    player.shoe = rubberSole;

    player.personas.push_back(new Orpheus);
    player.personas.push_back(new Forneus);

    player.curPersona = player.personas.at(0);

    /*--------------YUKARI-------------*/

    yukari.name = "Yukari";
    yukari.maxHp = 51;
    yukari.hp = 51;
    yukari.maxSp = 45;
    yukari.sp = 45;
    yukari.lv = 1;

    yukari.baseAttackAction = new Pierce_Attack;
    yukari.participantType = ParticipantType::Party;

    yukari.armourType = ArmourType::Female;
    yukari.armour = rashGuard;
    yukari.shoe = rubberSole;

    yukari.personas.push_back(new Io);

    yukari.curPersona = yukari.personas.at(0);

    /*--------------JUNPEI-------------*/
    junpei.name = "Junpei";
    junpei.maxHp = 70;
    junpei.hp = 70;
    junpei.maxSp = 36;
    junpei.sp = 36;
    junpei.lv = 1;

    junpei.baseAttackAction = new Slash_Attack;
    junpei.participantType = ParticipantType::Party;

    junpei.armourType = ArmourType::Male;
    junpei.armour = rashGuard;
    junpei.shoe = rubberSole;

    junpei.personas.push_back(new Hermes);

    junpei.curPersona = junpei.personas.at(0);
}
