#pragma once
#include <calico/types.h>

#define MENU_OPTIONS 7
#define SKILL_OPTIONS 9
#define ITEM_OPTIONS 3
#define EQUIP_OPTIONS 9
#define PERSONA_OPTIONS 3
#define STATS_OPTIONS 9
#define S_LINK_OPTIONS 3
#define SYSTEM_OPTIONS 6


typedef struct
{
    const char* name;
    bool selected;
    int bgIndex = -1;
} PauseOption;

// Menu options
enum {
    SKILL = 0,
    ITEM = 1,
    PERSONA = 2,
    EQUIP = 3,
    STATUS = 4,
    S_LINK = 5,
    SYSTEM = 6
};

// Skill/equip/stats options
enum {
    MAKOTO = 0,
    YUKARI = 1,
    JUNPEI = 2,
    AKIHIKO = 3,
    MITSURU = 4,
    AIGIS = 5,
    KEN = 6,
    KOROMARU = 7,
    SHINJIRO = 8
};

// Item options
enum {
    LIFE_STONE = 0,
    MEDICINE = 1,
    BEAD = 2
};

// Persona options
enum {
    JACK_FROST = 0,
    BLACK_FROST = 1,
    KING_FROST = 2
};

// S.Link options
enum {
    FOOL = 0,
    MAGICIAN = 1,
    EMPEROR = 2
};

// System options
enum {
    TUTORIAL = 0,
    CONFIG = 1,
    DICTIONARY = 2,
    LOAD_DATA = 3,
    SAVE_DATA = 4,
    RETURN_TO_TITLE = 5
};

// implementing from View
class PauseMenuComponent {
    private:
        mm_sfxhand sfxMenuHandle;
        mm_sfxhand sfxSelectHandle;
        mm_sfxhand sfxCancelHandle;

        PauseOption *options;
        int optionCount;
        int selectedOption = 0;

        PauseOption menuOptions[MENU_OPTIONS] = 
        {
            {"Skill", false},
            {"Item", false},
            {"Persona", false},
            {"Equip", false},
            {"Status", false},
            {"S.Link", false},
            {"System", false},
        };

        // TODO: go into submenus
        PauseOption skillOptions[SKILL_OPTIONS] = 
        {
            {"Makoto", false, 0},
            {"Yukari", false, 1},
            {"Junpei", false, 3},
            {"Akihiko", false, 2},
            {"Mitsuru", false},
            {"Aigis", false},
            {"Ken", false},
            {"Koromaru", false},
            {"Shinjiro", false},
        };

        PauseOption itemOptions[ITEM_OPTIONS] = 
        {
            {"Life Stone", false},
            {"Medicine", false},
            {"Bead", false},
        };

        // TODO: go into submenus
        PauseOption equipOptions[EQUIP_OPTIONS] = 
        {
            {"Makoto", false},
            {"Yukari", false},
            {"Junpei", false},
            {"Akihiko", false},
            {"Mitsuru", false},
            {"Aigis", false},
            {"Ken", false},
            {"Koromaru", false},
            {"Shinjiro", false},
        };

        PauseOption personaOptions[PERSONA_OPTIONS] = 
        {
            {"Jack Frot", false},
            {"Black Frost", false},
            {"King Frost", false},
        };

        // TODO: go into submenus
        PauseOption statsOptions[STATS_OPTIONS] = 
        {
            {"Makoto", false},
            {"Yukari", false},
            {"Junpei", false},
            {"Akihiko", false},
            {"Mitsuru", false},
            {"Aigis", false},
            {"Ken", false},
            {"Koromaru", false},
            {"Shinjiro", false},
        };

        // TODO: go into submenus
        PauseOption sLinkOptions[S_LINK_OPTIONS] =
        {
            {"Fool", false},
            {"Magician", false},
            {"Emperor", false},
        };

        // TODO: go into submenus
        PauseOption systemOptions[SYSTEM_OPTIONS] = 
        {
            {"Tutorial", false},
            {"Config", false},
            {"Dictionary", false},
            {"Load Data", false},
            {"Save Data", false},
            {"Return to Title", false},
        };
    public:
        void init(int iBgSlot);
        void update(int keys);
        void cancelSFX();
};