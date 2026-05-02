#include <nds.h>
#include "core/globals.h"
#include "PauseMenuComponent.h"

// sfx
#include "soundbank.h"

void PauseMenuComponent::cancelSFX() {
    musicCtrl.stopSFX(sfxMenuHandle);
    musicCtrl.stopSFX(sfxSelectHandle);
    musicCtrl.stopSFX(sfxCancelHandle);
}

void PauseMenuComponent::init() {
    // point to music
    musicCtrl.loadSFX(SFX_MENU);
    musicCtrl.loadSFX(SFX_SELECT);
    musicCtrl.loadSFX(SFX_CANCEL);

    // set default options
    options = menuOptions;
    optionCount = MENU_OPTIONS;
}

void PauseMenuComponent::update(int keys)
{
    // navigate options
    if (keys & KEY_DOWN)
    {
        sfxMenuHandle = musicCtrl.playSFX(SFX_MENU, 255, 128);
        selectedOption = (selectedOption + 1) % optionCount;
    }

    if (keys & KEY_UP)
    {
        sfxMenuHandle = musicCtrl.playSFX(SFX_MENU, 255, 128);
        selectedOption = (selectedOption + optionCount - 1) % optionCount;
    }

    if (keys & KEY_A)
    {
        sfxSelectHandle = musicCtrl.playSFX(SFX_SELECT, 255, 128);
        options[selectedOption].selected = true;
    }

    if (keys & KEY_B)
    {
        sfxCancelHandle = musicCtrl.playSFX(SFX_CANCEL, 255, 128);
        options[selectedOption].selected = false;
        // if we're in a submenu, return to main menu
        if (options != menuOptions)
        {
            options = menuOptions;
            optionCount = MENU_OPTIONS;
        }
    }

    consoleClear();

    // MENU OPTIONS
    if (menuOptions[SKILL].selected)
    { // -> Skill options
        cancelSFX();
        musicCtrl.playSFX(SFX_SELECT, 255, 128);

        menuOptions[SKILL].selected = false;
        selectedOption = 0;

        options = skillOptions;
        optionCount = SKILL_OPTIONS;
    }
    else if (menuOptions[ITEM].selected)
    { // -> Item options
        cancelSFX();
        musicCtrl.playSFX(SFX_SELECT, 255, 128);

        menuOptions[ITEM].selected = false;
        selectedOption = 0;

        options = itemOptions;
        optionCount = ITEM_OPTIONS;
    }
    else if (menuOptions[PERSONA].selected)
    { // -> Persona options
        cancelSFX();
        musicCtrl.playSFX(SFX_SELECT, 255, 128);

        menuOptions[PERSONA].selected = false;
        selectedOption = 0;

        options = personaOptions;
        optionCount = PERSONA_OPTIONS;
    }
    else if (menuOptions[EQUIP].selected)
    { // -> Equip options
        cancelSFX();
        musicCtrl.playSFX(SFX_SELECT, 255, 128);

        menuOptions[EQUIP].selected = false;
        selectedOption = 0;

        options = equipOptions;
        optionCount = EQUIP_OPTIONS;
    }
    else if (menuOptions[STATUS].selected)
    { // -> Status options
        cancelSFX();
        musicCtrl.playSFX(SFX_SELECT, 255, 128);

        menuOptions[STATUS].selected = false;
        selectedOption = 0;

        options = statsOptions;
        optionCount = STATS_OPTIONS;
    }
    else if (menuOptions[S_LINK].selected)
    { // -> S.Link options
        cancelSFX();
        musicCtrl.playSFX(SFX_SELECT, 255, 128);

        menuOptions[S_LINK].selected = false;
        selectedOption = 0;

        options = sLinkOptions;
        optionCount = S_LINK_OPTIONS;
    }
    else if (menuOptions[SYSTEM].selected)
    { // -> System options
        cancelSFX();
        musicCtrl.playSFX(SFX_SELECT, 255, 128);

        menuOptions[SYSTEM].selected = false;
        selectedOption = 0;

        options = systemOptions;
        optionCount = SYSTEM_OPTIONS;
    }
    // SKILL OPTIONS
    else if (skillOptions[MAKOTO].selected)
    { // "Makoto"
        cancelSFX();
        musicCtrl.playSFX(SFX_SELECT, 255, 128);
        return;
    }
    else if (skillOptions[YUKARI].selected)
    { // "Yukari"
        cancelSFX();
        musicCtrl.playSFX(SFX_SELECT, 255, 128);
        return;
    }
    else if (skillOptions[JUNPEI].selected)
    { // "Junpei"
        cancelSFX();
        musicCtrl.playSFX(SFX_SELECT, 255, 128);
        return;
    }
    else if (skillOptions[AKIHIKO].selected)
    { // "Akihiko"
        cancelSFX();
        musicCtrl.playSFX(SFX_SELECT, 255, 128);
        return;
    }
    else if (skillOptions[MITSURU].selected)
    { // "Mitsuru"
        cancelSFX();
        musicCtrl.playSFX(SFX_SELECT, 255, 128);
        return;
    }
    else if (skillOptions[AIGIS].selected)
    { // "Aigis"
        cancelSFX();
        musicCtrl.playSFX(SFX_SELECT, 255, 128);
        return;
    }
    else if (skillOptions[KEN].selected)
    { // "Ken"
        cancelSFX();
        musicCtrl.playSFX(SFX_SELECT, 255, 128);
        return;
    }
    else if (skillOptions[KOROMARU].selected)
    { // "Koromaru"
        cancelSFX();
        musicCtrl.playSFX(SFX_SELECT, 255, 128);
        return;
    }
    else if (skillOptions[SHINJIRO].selected)
    { // "Shinjiro"
        cancelSFX();
        musicCtrl.playSFX(SFX_SELECT, 255, 128);
        return;
    }
    // ITEM OPTIONS
    else if (itemOptions[LIFE_STONE].selected)
    { // "Life Stone"
        cancelSFX();
        musicCtrl.playSFX(SFX_SELECT, 255, 128);
        return;
    }
    else if (itemOptions[MEDICINE].selected)
    { // "Medicine"
        cancelSFX();
        musicCtrl.playSFX(SFX_SELECT, 255, 128);
        return;
    }
    else if (itemOptions[BEAD].selected)
    { // "Bead"
        cancelSFX();
        musicCtrl.playSFX(SFX_SELECT, 255, 128);
        return;
    }
    // EQUIP OPTINS
    //...
    // PERSONA OPTIONS
    else if (personaOptions[JACK_FROST].selected)
    { // "Jack Frost"
        cancelSFX();
        musicCtrl.playSFX(SFX_SELECT, 255, 128);
        return;
    }
    else if (personaOptions[BLACK_FROST].selected)
    { // "Black Frost"
        cancelSFX();
        musicCtrl.playSFX(SFX_SELECT, 255, 128);
        return;
    }
    else if (personaOptions[KING_FROST].selected)
    { // "King Frost"
        cancelSFX();
        musicCtrl.playSFX(SFX_SELECT, 255, 128);
        return;
    }
    // STATS OPTIONS
    //...
    // SLINK OPTIONS
    else if (sLinkOptions[FOOL].selected)
    { // "Fool"
        cancelSFX();
        musicCtrl.playSFX(SFX_SELECT, 255, 128);
        return;
    }
    else if (sLinkOptions[MAGICIAN].selected)
    { // "Magician"
        cancelSFX();
        musicCtrl.playSFX(SFX_SELECT, 255, 128);
        return;
    }
    else if (sLinkOptions[EMPEROR].selected)
    { // "Emperor"
        cancelSFX();
        musicCtrl.playSFX(SFX_SELECT, 255, 128);
        return;
    }
    // SYSTEM OPTIONS
    else if (systemOptions[TUTORIAL].selected)
    { // "Tutorial"
        cancelSFX();
        musicCtrl.playSFX(SFX_SELECT, 255, 128);
        return;
    }
    else if (systemOptions[CONFIG].selected)
    { // "Config"
        cancelSFX();
        musicCtrl.playSFX(SFX_SELECT, 255, 128);
        return;
    }
    else if (systemOptions[DICTIONARY].selected)
    { // "Dictionary"
        cancelSFX();
        musicCtrl.playSFX(SFX_SELECT, 255, 128);
        return;
    }
    else if (systemOptions[LOAD_DATA].selected)
    { // "Load Data"
        cancelSFX();
        musicCtrl.playSFX(SFX_SELECT, 255, 128);
        return;
    }
    else if (systemOptions[SAVE_DATA].selected)
    { // "Save Data"
        cancelSFX();
        musicCtrl.playSFX(SFX_SELECT, 255, 128);
        return;
    }
    else if (systemOptions[RETURN_TO_TITLE].selected)
    { // "Return to Title"
        cancelSFX();
        musicCtrl.playSFX(SFX_SELECT, 255, 128);
        return;
    }

    printf("\x1b[0;0H");

    // blink the "Pause" text
    if (frame % 60 < 30) {
        iprintf("Pause\n");
    } else {
        iprintf("\n");
    }

    // display options
    for (int option = 0; option < optionCount; option++)
    {
        iprintf("%c %s\n", option == selectedOption ? '>' : ' ', options[option].name);
    }

    return;
}