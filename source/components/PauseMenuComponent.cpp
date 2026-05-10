#include <nds.h>
#include "core/globals.h"
#include "PauseMenuComponent.h"

// sfx
#include "soundbank.h"
// backgrounds
#include "bgYukiClose.h"
#include "bgGuard.h"
#include "bgAkihiko.h"
#include "bgYuki.h"
// dialogue
#include "dialogue/demo_dialogue.h"

DialogueController dialogueCtrl;

// add background setup here, and assign the bgLoader index to the appropriate PauseOption in PauseMenuComponent.h
// NOTE: we can easily pass in custom bgLoaders. The reason I'm not implementing this behaviour is because I don't
// think they need to be different across interactions. The PauseMenuComponent should remain the same between
// different views
void PauseMenuComponent::setBgLoaders()
{
    bgLoaders[0] = [](int slot)
    {
        dmaCopy(bgGuardTiles, bgGetGfxPtr(slot), bgGuardTilesLen);
        dmaCopy(bgGuardMap, bgGetMapPtr(slot), bgGuardMapLen);
        vramSetBankH(VRAM_H_LCD);
        dmaCopy(bgGuardPal, &VRAM_H_EXT_PALETTE[0][0], bgGuardPalLen);
        vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);
        bgShow(slot);
    };
    bgLoaders[1] = [](int slot)
    {
        dmaCopy(bgYukiTiles, bgGetGfxPtr(slot), bgYukiTilesLen);
        dmaCopy(bgYukiMap, bgGetMapPtr(slot), bgYukiMapLen);
        vramSetBankH(VRAM_H_LCD);
        dmaCopy(bgYukiPal, &VRAM_H_EXT_PALETTE[0][0], bgYukiPalLen);
        vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);
        bgShow(slot);
    };
    bgLoaders[2] = [](int slot)
    {
        dmaCopy(bgAkihikoTiles, bgGetGfxPtr(slot), bgAkihikoTilesLen);
        dmaCopy(bgAkihikoMap, bgGetMapPtr(slot), bgAkihikoMapLen);
        vramSetBankH(VRAM_H_LCD);
        dmaCopy(bgAkihikoPal, &VRAM_H_EXT_PALETTE[0][0], bgAkihikoPalLen);
        vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);
        bgShow(slot);
    };
    bgLoaders[3] = [](int slot)
    {
        dmaCopy(bgYukiCloseTiles, bgGetGfxPtr(slot), bgYukiCloseTilesLen);
        dmaCopy(bgYukiCloseMap, bgGetMapPtr(slot), bgYukiCloseMapLen);
        vramSetBankH(VRAM_H_LCD);
        dmaCopy(bgYukiClosePal, &VRAM_H_EXT_PALETTE[0][0], bgYukiClosePalLen);
        vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);
        bgShow(slot);
    };
}

void PauseMenuComponent::loadBg(int bgIndex)
{
    if (bgIndex >= 0 && bgIndex < 4 && bgLoaders[bgIndex])
        bgLoaders[bgIndex](bgSlot);
}

void PauseMenuComponent::cancelSFX()
{
    musicCtrl.stopSFX(sfxMenuHandle);
    musicCtrl.stopSFX(sfxSelectHandle);
    musicCtrl.stopSFX(sfxCancelHandle);
}

void PauseMenuComponent::init(int iBgSlot, bool* isActive)
{
    // point to music
    musicCtrl.loadSFX(SFX_MENU);
    musicCtrl.loadSFX(SFX_SELECT);
    musicCtrl.loadSFX(SFX_CANCEL);

    // set default options
    options = menuOptions;
    optionCount = MENU_OPTIONS;
    selectedOption = 0;

    // set bg loaders
    bgSlot = iBgSlot;
    setBgLoaders();

    isActivePtr = isActive;

    // initialize view state
    nextViewState = ViewState::KEEP_CURRENT;
}

ViewState PauseMenuComponent::update(int keys)
{
    // added for debug testing of dialogue
    if (dialogueCtrl.isActive()) {
        dialogueCtrl.update(keys);
        return ViewState::KEEP_CURRENT;
    }

    // navigate options
    if (keys & KEY_DOWN)
    {
        sfxMenuHandle = musicCtrl.playSFX(SFX_MENU, 255, 128);
        selectedOption = (selectedOption + 1) % optionCount;
    }
    else if (keys & KEY_UP)
    {
        sfxMenuHandle = musicCtrl.playSFX(SFX_MENU, 255, 128);
        selectedOption = (selectedOption + optionCount - 1) % optionCount;
    }
    else if (keys & KEY_A)
    {
        cancelSFX();
        sfxSelectHandle = musicCtrl.playSFX(SFX_SELECT, 255, 128);

        PauseState currentState = {options, optionCount, selectedOption};

        if (options[selectedOption].onSelect != nullptr)
        {
            ViewState result = (this->*(options[selectedOption].onSelect))();
            if (result != ViewState::KEEP_CURRENT) {
                nextViewState = result;
                *isActivePtr = false;
            } else if (dialogueCtrl.isActive()) {   // added for debug testing of dialogue
                 return ViewState::KEEP_CURRENT;
            }
            // if we changed options, push current state to stack
            if (options != currentState.options)
            {
                prevOptions.push(currentState);
            }
        }
    }

    if (keys & KEY_B)
    {
        cancelSFX();
        musicCtrl.playSFX(SFX_CANCEL, 255, 128);
        selectedOption = 0;

        // if we're in a submenu, return to main menu
        if (!prevOptions.empty())
        {
            PauseState prevState = prevOptions.top();
            prevOptions.pop();

            options = prevState.options;
            optionCount = prevState.optionCount;
            selectedOption = prevState.selectedOption;
        } else {
            // otherwise, close the menu
            *isActivePtr = false;
        }
    }

    consoleClear();
    printf("\x1b[0;0H");

    // blink the "Pause" text
    if (frame % 60 < 30)
    {
        iprintf("Pause\n");
    }
    else
    {
        iprintf("\n");
    }

    // display options
    for (int option = 0; option < optionCount; option++)
    {
        iprintf("%c %s\n", option == selectedOption ? '>' : ' ', options[option].name);
    }

    // load selectedOption's background
    int bgIndex = options[selectedOption].bgIndex;
    if (bgIndex != -1)
    {
        loadBg(bgIndex);
        bgShow(bgSlot);
    }
    else
    {
        bgHide(bgSlot);
    }

    ViewState viewState = nextViewState;
    nextViewState = ViewState::KEEP_CURRENT;
    return viewState;
}

// OPTION HANDLERS
// menuOption handlers
ViewState PauseMenuComponent::openSkillMenu()
{
    selectedOption = 0;
    options = skillOptions;
    optionCount = SKILL_OPTIONS;
    return ViewState::KEEP_CURRENT;
}

ViewState PauseMenuComponent::openItemMenu()
{
    selectedOption = 0;
    options = itemOptions;
    optionCount = ITEM_OPTIONS;
    return ViewState::KEEP_CURRENT;
}

ViewState PauseMenuComponent::openPersonaMenu()
{
    selectedOption = 0;
    options = personaOptions;
    optionCount = PERSONA_OPTIONS;
    return ViewState::KEEP_CURRENT;
}

ViewState PauseMenuComponent::openEquipMenu()
{
    selectedOption = 0;
    options = equipOptions;
    optionCount = EQUIP_OPTIONS;
    return ViewState::KEEP_CURRENT;
}

ViewState PauseMenuComponent::openStatusMenu()
{
    selectedOption = 0;
    options = statsOptions;
    optionCount = STATS_OPTIONS;
    return ViewState::KEEP_CURRENT;
}

ViewState PauseMenuComponent::openSLinkMenu()
{
    selectedOption = 0;
    options = sLinkOptions;
    optionCount = S_LINK_OPTIONS;
    return ViewState::KEEP_CURRENT;
}

ViewState PauseMenuComponent::openSystemMenu()
{
    selectedOption = 0;
    options = systemOptions;
    optionCount = SYSTEM_OPTIONS;
    return ViewState::KEEP_CURRENT;
}

// Debug options
ViewState PauseMenuComponent::openDebugMenu()
{
    selectedOption = 0;
    options = debugOptions;
    optionCount = DEBUG_OPTIONS;
    return ViewState::KEEP_CURRENT;
}

ViewState PauseMenuComponent::openCharacterAnimMenu()
{
    selectedOption = 0;
    options = characterAnimOptions;
    optionCount = CHARACTER_ANIM_OPTIONS;
    return ViewState::KEEP_CURRENT;
}


// generic handlers
// this is where we would implement functionality for going into a sub-menu, or selecting a skill, item, etc.
ViewState PauseMenuComponent::skillOptionSelected()
{
    selectedOption = 0;
    options = skills;
    optionCount = SKILLS;
    return ViewState::KEEP_CURRENT;
}

ViewState PauseMenuComponent::itemOptionSelected()
{
    // ...
    return ViewState::KEEP_CURRENT;
}

ViewState PauseMenuComponent::equipOptionSelected()
{
    // ...
    return ViewState::KEEP_CURRENT;
}

ViewState PauseMenuComponent::personaOptionSelected()
{
    // ...
    return ViewState::KEEP_CURRENT;
}

ViewState PauseMenuComponent::statsOptionSelected()
{
    // ...
    return ViewState::KEEP_CURRENT;
}

ViewState PauseMenuComponent::sLinkOptionSelected()
{
    // ...
    return ViewState::KEEP_CURRENT;
}

ViewState PauseMenuComponent::systemOptionSelected()
{
    // ...
    return ViewState::KEEP_CURRENT;
}

// Debug options
ViewState PauseMenuComponent::debugOptionSelected()
{
    // set the next view state based on the selected debug option
    ViewState selectedView;
    switch (selectedOption)
    {
        case DISCLAIMER_VIEW:
            selectedView = ViewState::DISCLAIMER;
            break;
        case INTRO_VIDEO_VIEW:
            selectedView = ViewState::INTRO_VIDEO;
            break;
        case INTRO_VIEW:
            selectedView = ViewState::INTRO;
            break;
        case MAIN_MENU_VIEW:
            selectedView = ViewState::MAIN_MENU;
            break;
        case IWATODAI_DORM_VIEW:
            selectedView = ViewState::IWATODAI_DORM;
            break;
        case IWATODAI_STREETS_VIEW:
            selectedView = ViewState::IWATODAI_STREETS;
            break;
        case DEBUG_DIALOGUE:
            consoleClear();
            demo_yuki_guard_argument_load();
            dialogueCtrl.setLoader(demo_yuki_guard_argument_load_bg);
            dialogueCtrl.start(demo_yuki_guard_argument_first());
            selectedView = ViewState::KEEP_CURRENT;
            break;
        case TOGGLE_BILLBOARDS:
            enableBillboards = !enableBillboards;
            *isActivePtr = false;
            selectedView = ViewState::KEEP_CURRENT;
            break;
        case TOGGLE_DEBUG_PRINT:
            enableDebugPrint = true;
            *isActivePtr = false;
            selectedView = ViewState::KEEP_CURRENT;
        default:
            selectedView = ViewState::KEEP_CURRENT;
    }
    return selectedView;
}

ViewState PauseMenuComponent::characterAnimOptionSelected()
{
    // stop currently playing animation
    characterAnimationCtrl.stop();

    // set the next view state based on the selected debug option
    ViewState selectedView;
    switch (selectedOption)
    {
        case TOGGLE_AUTO_ANIM:
            enableCharacterAnim = !enableCharacterAnim;
            break;
        // TODO: update to new animation enums
        // case ANIM_1:
        //     characterAnimationCtrl.set(MODEL_CHARACTER_1, true);
        //     enableCharacterAnim = false;
        //     break;
        // case ANIM_2:
        //     characterAnimationCtrl.set(MODEL_CHARACTER_2, true);
        //     enableCharacterAnim = false;
        //     break;
        // case ANIM_3:
        //     characterAnimationCtrl.set(MODEL_CHARACTER_3, true);
        //     enableCharacterAnim = false;
        //     break;
        // case ANIM_4:
        //     characterAnimationCtrl.set(MODEL_CHARACTER_4, true);
        //     enableCharacterAnim = false;
        //     break;
        // case ANIM_5:
        //     characterAnimationCtrl.set(MODEL_CHARACTER_5, true);
        //     enableCharacterAnim = false;
        //     break;
        // case ANIM_6:
        //     characterAnimationCtrl.set(MODEL_CHARACTER_6, true);
        //     enableCharacterAnim = false;
        //     break;
        // case ANIM_7:
        //     characterAnimationCtrl.set(MODEL_CHARACTER_7, true);
        //     enableCharacterAnim = false;
        //     break;
        // case ANIM_8:
        //     characterAnimationCtrl.set(MODEL_CHARACTER_8, true);
        //     enableCharacterAnim = false;
        //     break;
        // case ANIM_9:
        //     characterAnimationCtrl.set(MODEL_CHARACTER_9, true);
        //     enableCharacterAnim = false;
        //     break;
        // case ANIM_10:
        //     characterAnimationCtrl.set(MODEL_CHARACTER_10, true);
        //     enableCharacterAnim = false;
        //     break;
        // case ANIM_11:
        //     characterAnimationCtrl.set(MODEL_CHARACTER_11, true);
        //     enableCharacterAnim = false;
        //     break;
        // case ANIM_12:
        //     characterAnimationCtrl.set(MODEL_CHARACTER_12, true);
        //     enableCharacterAnim = false;
        //     break;
        // case ANIM_13:
        //     characterAnimationCtrl.set(MODEL_CHARACTER_13, true);
        //     enableCharacterAnim = false;
        //     break;
        // case ANIM_14:
        //     characterAnimationCtrl.set(MODEL_CHARACTER_14, true);
        //     enableCharacterAnim = false;
        //     break;
        // case ANIM_15:
        //     characterAnimationCtrl.set(MODEL_CHARACTER_15, true);
        //     enableCharacterAnim = false;
        //     break;
        default:
            selectedView = ViewState::KEEP_CURRENT;
    }

    *isActivePtr = false;
    characterAnimationCtrl.play();
    return selectedView;
}

