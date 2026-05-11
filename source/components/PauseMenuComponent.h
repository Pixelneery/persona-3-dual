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
#define DEBUG_OPTIONS 10
#define CHARACTER_ANIM_OPTIONS 25
#define SKILLS 2

class PauseMenuComponent;
typedef struct
{
    const char *name;
    int bgIndex;
    ViewState (PauseMenuComponent::*onSelect)();
} PauseOption;

typedef struct
{
    PauseOption *options;
    int optionCount;
    int selectedOption;
    int startIndex;
} PauseState;

// Menu options
enum
{
    SKILL = 0,
    ITEM = 1,
    PERSONA = 2,
    EQUIP = 3,
    STATUS = 4,
    S_LINK = 5,
    SYSTEM = 6
};

// Skill/equip/stats options
enum
{
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
enum
{
    LIFE_STONE = 0,
    MEDICINE = 1,
    BEAD = 2
};

// Persona options
enum
{
    JACK_FROST = 0,
    BLACK_FROST = 1,
    KING_FROST = 2
};

// S.Link options
enum
{
    FOOL = 0,
    MAGICIAN = 1,
    EMPEROR = 2
};

// System options
enum
{
    TUTORIAL = 0,
    CONFIG = 1,
    DICTIONARY = 2,
    LOAD_DATA = 3,
    SAVE_DATA = 4,
    RETURN_TO_TITLE = 5
};

// Debug options
enum
{
    DISCLAIMER_VIEW = 0,
    INTRO_VIDEO_VIEW = 1,
    INTRO_VIEW = 2,
    MAIN_MENU_VIEW = 3,
    IWATODAI_DORM_VIEW = 4,
    IWATODAI_STREETS_VIEW = 5,
    DEBUG_DIALOGUE = 6,
    TOGGLE_BILLBOARDS = 7,
    TOGGLE_DEBUG_PRINT = 8,
    PLAY_CHARACTER_ANIM = 9
};

// Character animation opitons
enum
{
    TOGGLE_AUTO_ANIM = 0,
    ANIM_1 = 1,
    ANIM_2 = 2,
    ANIM_3 = 3,
    ANIM_4 = 4,
    ANIM_5 = 5,
    ANIM_6 = 6,
    ANIM_7 = 7,
    ANIM_8 = 8,
    ANIM_9 = 9,
    ANIM_10 = 10,
    ANIM_11 = 11,
    ANIM_12 = 12,
    ANIM_13 = 13,
    ANIM_14 = 14,
    ANIM_15 = 15,
    ANIM_16 = 16,
    ANIM_17 = 17,
    ANIM_18 = 18,
    ANIM_19 = 19,
    ANIM_20 = 20,
    ANIM_21 = 21,
    ANIM_22 = 22,
    ANIM_23 = 23,
    ANIM_24 = 24
};

class PauseMenuComponent
{
private:
    bool *isActivePtr;

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
    int startIndex = 0;
    int visibleOptions = 23;
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
            {"IwatodaiStreetView", -1, &PauseMenuComponent::debugOptionSelected},
            {"Debug Dialogue", -1, &PauseMenuComponent::debugOptionSelected},
            {"Toggle Billboards", -1, &PauseMenuComponent::debugOptionSelected},
            {"Toggle Debug Print", -1, &PauseMenuComponent::debugOptionSelected},
            {"Play Character Animations", -1, &PauseMenuComponent::openCharacterAnimMenu},
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

    // submenu dummy options for testing
    PauseOption skills[SKILLS] =
        {
            {"Skill 1", -1, nullptr},
            {"Skill 2", -1, nullptr},
    };

    PauseOption characterAnimOptions[CHARACTER_ANIM_OPTIONS] =
        {
            {"Toggle Auto Animations", -1, &PauseMenuComponent::characterAnimOptionSelected},
            {"0000", -1, &PauseMenuComponent::characterAnimOptionSelected},
            {"0001", -1, &PauseMenuComponent::characterAnimOptionSelected},
            {"0002", -1, &PauseMenuComponent::characterAnimOptionSelected},
            {"0003", -1, &PauseMenuComponent::characterAnimOptionSelected},
            {"0004", -1, &PauseMenuComponent::characterAnimOptionSelected},
            {"0005", -1, &PauseMenuComponent::characterAnimOptionSelected},
            {"0006", -1, &PauseMenuComponent::characterAnimOptionSelected},
            {"0007", -1, &PauseMenuComponent::characterAnimOptionSelected},
            {"0008", -1, &PauseMenuComponent::characterAnimOptionSelected},
            {"0009", -1, &PauseMenuComponent::characterAnimOptionSelected},
            {"0010", -1, &PauseMenuComponent::characterAnimOptionSelected},
            {"0011", -1, &PauseMenuComponent::characterAnimOptionSelected},
            {"0012", -1, &PauseMenuComponent::characterAnimOptionSelected},
            {"0013", -1, &PauseMenuComponent::characterAnimOptionSelected},
            {"0014", -1, &PauseMenuComponent::characterAnimOptionSelected},
            {"0015", -1, &PauseMenuComponent::characterAnimOptionSelected},
            {"0016", -1, &PauseMenuComponent::characterAnimOptionSelected},
            {"0017", -1, &PauseMenuComponent::characterAnimOptionSelected},
            {"0018", -1, &PauseMenuComponent::characterAnimOptionSelected},
            {"0019", -1, &PauseMenuComponent::characterAnimOptionSelected},
            {"0020", -1, &PauseMenuComponent::characterAnimOptionSelected},
            {"0021", -1, &PauseMenuComponent::characterAnimOptionSelected},
            {"0022", -1, &PauseMenuComponent::characterAnimOptionSelected},
            {"root", -1, &PauseMenuComponent::characterAnimOptionSelected},
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
    ViewState characterAnimOptionSelected();
    // debug
    ViewState openCharacterAnimMenu();

public:
    void init(int iBgSlot, bool *isActive);
    ViewState update(int keys);
    void cancelSFX();
};
