#pragma once
#include "CharacterProfiles.h"

namespace CharacterProfiles
{
    CharacterProfile player;
    CharacterProfile yukari;
    CharacterProfile junpei;

    void InitializeProfiles()
    {

        RashGuard rashGuard;

        /*--------------MC-----------------*/

        player.name = "Player";
        player.hp = 72;
        player.sp = 50;
        player.lv = 2;

        player.baseAttackAction = new Slash_Attack;
        player.participantType = ParticipantType::Player;

        PlainShirt plainShirt;
        player.armourType = ArmourType::Male;
        player.armour = plainShirt;

        player.personas.push_back(new Orpheus);
        player.personas.push_back(new Forneus);

        player.curPersona = player.personas.at(0);

        /*--------------YUKARI-------------*/

        yukari.name = "Yukari";
        yukari.hp = 51;
        yukari.sp = 45;
        yukari.lv = 1;

        yukari.baseAttackAction = new Pierce_Attack;
        yukari.participantType = ParticipantType::Party;

        yukari.armourType = ArmourType::Female;
        yukari.armour = rashGuard;

        yukari.personas.push_back(new Io);

        yukari.curPersona = yukari.personas.at(0);

        /*--------------JUNPEI-------------*/
        junpei.name = "Junpei";
        junpei.hp = 70;
        junpei.sp = 36;
        junpei.lv = 1;

        junpei.baseAttackAction = new Slash_Attack;
        junpei.participantType = ParticipantType::Party;

        junpei.armourType = ArmourType::Male;
        junpei.armour = rashGuard;

        junpei.personas.push_back(new Hermes);

        junpei.curPersona = junpei.personas.at(0);
    }
};
