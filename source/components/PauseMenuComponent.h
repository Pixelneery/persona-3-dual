#pragma once
#include <calico/types.h>
#include <stack>

#define MENU_OPTIONS 8
#define SKILL_OPTIONS 9
#define ITEM_OPTIONS 3
#define EQUIP_OPTIONS 9
#define PERSONA_OPTIONS 3
#define STATS_OPTIONS 9
#define S_LINK_OPTIONS 3
#define SYSTEM_OPTIONS 6

// dummy option for testing
#define DEBUG_OPTIONS 5
# define SKILLS 2

class PauseMenuComponent;
typedef struct
{
    const char* name;
    int bgIndex;
    ViewState (PauseMenuComponent::*onSelect)();
} PauseOption;

typedef struct
{
    PauseOption *options;
    int optionCount;
    int selectedOption;
} PauseState;


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

// Debug options
enum {
    DISCLAIMER_VIEW = 0,
    INTRO_VIDEO_VIEW = 1,
    INTRO_VIEW = 2,
    MAIN_MENU_VIEW = 3,
    IWATODAI_DORM_VIEW = 4
};

class PauseMenuComponent {
    private:
        // sfx
        mm_sfxhand sfxMenuHandle;
        mm_sfxhand sfxSelectHandle;
        mm_sfxhand sfxCancelHandle;

        // backgrounds
        int bgSlot = 0;
        void (*bgLoaders[4])(int) = {
            nullptr,
            nullptr,
            nullptr,
            nullptr,
        };

        // options
        PauseOption *options;
        stack<PauseState> prevOptions;
        int optionCount = 0;
        int selectedOption = 0;
        ViewState nextViewState = ViewState::KEEP_CURRENT;

        PauseOption menuOptions[MENU_OPTIONS] = 
        {
            {"Debug", -1, &PauseMenuComponent::openDebugMenu},
            {"Skill", -1, &PauseMenuComponent::openSkillMenu},
            {"Item", -1, &PauseMenuComponent::openItemMenu},
            {"Persona", -1, &PauseMenuComponent::openPersonaMenu},
            {"Equip", -1, &PauseMenuComponent::openEquipMenu},
            {"Status", -1, &PauseMenuComponent::openStatusMenu},
            {"S.Link", -1, &PauseMenuComponent::openSLinkMenu},
            {"System", -1, &PauseMenuComponent::openSystemMenu},
        };

        PauseOption debugOptions[DEBUG_OPTIONS] = 
        {
            {"DisclaimerView", -1, &PauseMenuComponent::debugOptionSelected},
            {"IntroVideoView", -1, &PauseMenuComponent::debugOptionSelected},
            {"IntroView", -1, &PauseMenuComponent::debugOptionSelected},
            {"MainMenuView", -1, &PauseMenuComponent::debugOptionSelected},
            {"IwatodaiDormView", -1, &PauseMenuComponent::debugOptionSelected},
        };

        // TODO: go into submenus
        PauseOption skillOptions[SKILL_OPTIONS] = 
        {
            {"Makoto", 0, &PauseMenuComponent::skillOptionSelected},
            {"Yukari", 1, &PauseMenuComponent::skillOptionSelected},
            {"Junpei", 3, &PauseMenuComponent::skillOptionSelected},
            {"Akihiko", 2, &PauseMenuComponent::skillOptionSelected},
            {"Mitsuru", -1, &PauseMenuComponent::skillOptionSelected},
            {"Aigis", -1, &PauseMenuComponent::skillOptionSelected},
            {"Ken", -1, &PauseMenuComponent::skillOptionSelected},
            {"Koromaru", -1, &PauseMenuComponent::skillOptionSelected},
            {"Shinjiro", -1, &PauseMenuComponent::skillOptionSelected},
        };

        PauseOption itemOptions[ITEM_OPTIONS] = 
        {
            {"Life Stone", -1, &PauseMenuComponent::itemOptionSelected},
            {"Medicine", -1, &PauseMenuComponent::itemOptionSelected},
            {"Bead", -1, &PauseMenuComponent::itemOptionSelected},
        };

        // TODO: go into submenus
        PauseOption equipOptions[EQUIP_OPTIONS] = 
        {
            {"Makoto", -1, &PauseMenuComponent::equipOptionSelected},
            {"Yukari", -1, &PauseMenuComponent::equipOptionSelected},
            {"Junpei", -1, &PauseMenuComponent::equipOptionSelected},
            {"Akihiko", -1, &PauseMenuComponent::equipOptionSelected},
            {"Mitsuru", -1, &PauseMenuComponent::equipOptionSelected},
            {"Aigis", -1, &PauseMenuComponent::equipOptionSelected},
            {"Ken", -1, &PauseMenuComponent::equipOptionSelected},
            {"Koromaru", -1, &PauseMenuComponent::equipOptionSelected},
            {"Shinjiro", -1, &PauseMenuComponent::equipOptionSelected},
        };

        PauseOption personaOptions[PERSONA_OPTIONS] = 
        {
            {"Jack Frost", -1, &PauseMenuComponent::personaOptionSelected},
            {"Black Frost", -1, &PauseMenuComponent::personaOptionSelected},
            {"King Frost", -1, &PauseMenuComponent::personaOptionSelected},
        };

        // TODO: go into submenus
        PauseOption statsOptions[STATS_OPTIONS] = 
        {
            {"Makoto", -1, &PauseMenuComponent::statsOptionSelected},
            {"Yukari", -1, &PauseMenuComponent::statsOptionSelected},
            {"Junpei", -1, &PauseMenuComponent::statsOptionSelected},
            {"Akihiko", -1, &PauseMenuComponent::statsOptionSelected},
            {"Mitsuru", -1, &PauseMenuComponent::statsOptionSelected},
            {"Aigis", -1, &PauseMenuComponent::statsOptionSelected},
            {"Ken", -1, &PauseMenuComponent::statsOptionSelected},
            {"Koromaru", -1, &PauseMenuComponent::statsOptionSelected},
            {"Shinjiro", -1, &PauseMenuComponent::statsOptionSelected},
        };

        // TODO: go into submenus
        PauseOption sLinkOptions[S_LINK_OPTIONS] =
        {
            {"Fool", -1, &PauseMenuComponent::sLinkOptionSelected},
            {"Magician", -1, &PauseMenuComponent::sLinkOptionSelected},
            {"Emperor", -1, &PauseMenuComponent::sLinkOptionSelected},
        };

        // TODO: go into submenus
        PauseOption systemOptions[SYSTEM_OPTIONS] = 
        {
            {"Tutorial", -1, &PauseMenuComponent::systemOptionSelected},
            {"Config", -1, &PauseMenuComponent::systemOptionSelected},
            {"Dictionary", -1, &PauseMenuComponent::systemOptionSelected},
            {"Load Data", -1, &PauseMenuComponent::systemOptionSelected},
            {"Save Data", -1, &PauseMenuComponent::systemOptionSelected},
            {"Return to Title", -1, &PauseMenuComponent::systemOptionSelected},
        };

        // dummy options for testing
        PauseOption skills[SKILLS] = 
        {
            {"Skill 1", -1, nullptr},
            {"Skill 2", -1, nullptr},
        };

        void setBgLoaders();
        void loadBg(int bgIndex);

        // OPTION HANDLERS
        // menuOption handlers
        ViewState openDebugMenu();
        ViewState openSkillMenu();
        ViewState openItemMenu();
        ViewState openPersonaMenu();
        ViewState openEquipMenu();
        ViewState openStatusMenu();
        ViewState openSLinkMenu();
        ViewState openSystemMenu();
        // generic handlers
        ViewState debugOptionSelected();
        ViewState skillOptionSelected();
        ViewState itemOptionSelected();
        ViewState equipOptionSelected();
        ViewState personaOptionSelected();
        ViewState statsOptionSelected();
        ViewState sLinkOptionSelected();
        ViewState systemOptionSelected();

    public:
        void init(int iBgSlot);
        ViewState update(int keys);
        void cancelSFX();
};