#include <nds.h>
#include "core/globals.h"
#include "PauseMenuComponent.h"
#include "models/character.h"

// sfx
#include "soundbank.h"
// backgrounds
#include "bgAkihiko.h"
#include "bgKenji.h"
#include "bgYukari.h"
#include "bgYukariClose.h"
// dialogue
#include "dialogue/demo_dialogue.h"

DialogueController dialogueCtrl;

void PauseMenuComponent::loadBg(int bgIndex)
{
    if (bgIndex < 0) return;

    switch (bgIndex)
    {
        case 0: // Akihiko
            dmaCopy(bgAkihikoTiles, bgGetGfxPtr(bgSlot), bgAkihikoTilesLen);
            dmaCopy(bgAkihikoMap, bgGetMapPtr(bgSlot), bgAkihikoMapLen);
            vramSetBankH(VRAM_H_LCD);
            dmaCopy(bgAkihikoPal, &VRAM_H_EXT_PALETTE[0][0], bgAkihikoPalLen);
            vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);
            break;

        case 1: // Kenji
            dmaCopy(bgKenjiTiles, bgGetGfxPtr(bgSlot), bgKenjiTilesLen);
            dmaCopy(bgKenjiMap, bgGetMapPtr(bgSlot), bgKenjiMapLen);
            vramSetBankH(VRAM_H_LCD);
            dmaCopy(bgKenjiPal, &VRAM_H_EXT_PALETTE[0][0], bgKenjiPalLen);
            vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);
            break;

        case 2: // Yukari
            dmaCopy(bgYukariTiles, bgGetGfxPtr(bgSlot), bgYukariTilesLen);
            dmaCopy(bgYukariMap, bgGetMapPtr(bgSlot), bgYukariMapLen);
            vramSetBankH(VRAM_H_LCD);
            dmaCopy(bgYukariPal, &VRAM_H_EXT_PALETTE[0][0], bgYukariPalLen);
            vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);
            break;

        case 3: // YukariClose
            dmaCopy(bgYukariCloseTiles, bgGetGfxPtr(bgSlot), bgYukariCloseTilesLen);
            dmaCopy(bgYukariCloseMap, bgGetMapPtr(bgSlot), bgYukariCloseMapLen);
            vramSetBankH(VRAM_H_LCD);
            dmaCopy(bgYukariClosePal, &VRAM_H_EXT_PALETTE[0][0], bgYukariClosePalLen);
            vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);
            break;

        default:
            break;
    }
}

void PauseMenuComponent::init(int iBgSlot, bool *isActive, const std::string &iPauseMessage)
{
    BaseMenu::init(iBgSlot, isActive, iPauseMessage);
    options = menuOptions;
    optionCount = MENU_OPTIONS;
}

ViewState PauseMenuComponent::update(int keys)
{
    // dialogue controller takes full control when active
    if (dialogueCtrl.isActive())
    {
        dialogueCtrl.update(keys);
        return ViewState::KEEP_CURRENT;
    }

    return BaseMenu::update(keys);
}

// menu navigation handlers

ViewState PauseMenuComponent::openDebugMenu()
{
    return changeMenu(debugOptions, DEBUG_OPTIONS);
}

ViewState PauseMenuComponent::openSkillMenu()
{
    return changeMenu(skillOptions, SKILL_OPTIONS);
}

ViewState PauseMenuComponent::openItemMenu()
{
    return changeMenu(itemOptions, ITEM_OPTIONS);
}

ViewState PauseMenuComponent::openPersonaMenu()
{
    return changeMenu(personaOptions, PERSONA_OPTIONS);
}

ViewState PauseMenuComponent::openEquipMenu()
{
    return changeMenu(equipOptions, EQUIP_OPTIONS);
}

ViewState PauseMenuComponent::openStatusMenu()
{
    return changeMenu(statsOptions, STATS_OPTIONS);
}

ViewState PauseMenuComponent::openSLinkMenu()
{
    return changeMenu(sLinkOptions, S_LINK_OPTIONS);
}

ViewState PauseMenuComponent::openSystemMenu()
{
    return changeMenu(systemOptions, SYSTEM_OPTIONS);
}

ViewState PauseMenuComponent::openCharacterAnimMenu()
{
    return changeMenu(characterAnimOptions, CHARACTER_ANIM_OPTIONS);
}

// selection handlers

ViewState PauseMenuComponent::skillOptionSelected()
{
    return changeMenu(skills, SKILLS);
}

ViewState PauseMenuComponent::itemOptionSelected()
{
    return ViewState::KEEP_CURRENT;
}

ViewState PauseMenuComponent::equipOptionSelected()
{
    return ViewState::KEEP_CURRENT;
}

ViewState PauseMenuComponent::personaOptionSelected()
{
    return ViewState::KEEP_CURRENT;
}

ViewState PauseMenuComponent::statsOptionSelected()
{
    return ViewState::KEEP_CURRENT;
}

ViewState PauseMenuComponent::sLinkOptionSelected()
{
    return ViewState::KEEP_CURRENT;
}

ViewState PauseMenuComponent::systemOptionSelected()
{
    return ViewState::KEEP_CURRENT;
}

ViewState PauseMenuComponent::debugOptionSelected()
{
    ViewState selectedView;
    switch (static_cast<DebugOption>(selectedOption))
    {
        case DebugOption::DISCLAIMER_VIEW:
            musicCtrl.pause();
            selectedView = ViewState::DISCLAIMER;
            break;
        case DebugOption::INTRO_VIEW:
            selectedView = ViewState::INTRO;
            break;
        case DebugOption::MAIN_MENU_VIEW:
            selectedView = ViewState::MAIN_MENU;
            break;
        case DebugOption::IWATODAI_DORM_VIEW:
            selectedView = ViewState::IWATODAI_DORM;
            break;
        case DebugOption::IWATODAI_STREETS_VIEW:
            selectedView = ViewState::IWATODAI_STREETS;
            break;
        case DebugOption::STATION_VIEW:
            selectedView = ViewState::STATION;
            break;
        case DebugOption::SIGN_CONTRACT_VIEW:
            selectedView = ViewState::SIGN_CONTRACT;
            break;
        case DebugOption::PAULOWNIA_MALL_VIEW:
            selectedView = ViewState::PAULOWNIA_MALL;
            break;
        case DebugOption::DEBUG_VIEW:
            selectedView = ViewState::DEBUG_VIEW;
            break;
        case DebugOption::INTRO_VIDEO:
            selectedView = ViewState::INTRO_VIDEO;
            break;
        case DebugOption::CUTSCENE_1:
            selectedView = ViewState::CUTSCENE_1;
            break;
        case DebugOption::CUTSCENE_2:
            selectedView = ViewState::CUTSCENE_2;
            break;
        case DebugOption::DEBUG_DIALOGUE:
            consoleClear();
            demo_yukari_kenji_argument_load();
            dialogueCtrl.setLoader(demo_yukari_kenji_argument_load_bg);
            dialogueCtrl.start(demo_yukari_kenji_argument_first());
            selectedView = ViewState::KEEP_CURRENT;
            break;
        case DebugOption::TOGGLE_BILLBOARDS:
            enableBillboards = !enableBillboards;
            *isActivePtr = false;
            selectedView = ViewState::KEEP_CURRENT;
            break;
        case DebugOption::TOGGLE_DEBUG_PRINT:
            enableDebugPrint = true;
            *isActivePtr = false;
            selectedView = ViewState::KEEP_CURRENT;
            break;
        default:
            selectedView = ViewState::KEEP_CURRENT;
    }
    return selectedView;
}

ViewState PauseMenuComponent::characterAnimOptionSelected()
{
    characterAnimationCtrl.stop();

    ViewState selectedView = ViewState::KEEP_CURRENT;
    switch (static_cast<CharacterAnimOption>(selectedOption))
    {
        case CharacterAnimOption::TOGGLE_AUTO_ANIM:
            enableCharacterAnim = !enableCharacterAnim;
            break;
        case CharacterAnimOption::ANIM_1:  characterAnimationCtrl.set((Model_character)0,  true); enableCharacterAnim = false; break;
        case CharacterAnimOption::ANIM_2:  characterAnimationCtrl.set((Model_character)1,  true); enableCharacterAnim = false; break;
        case CharacterAnimOption::ANIM_3:  characterAnimationCtrl.set((Model_character)2,  true); enableCharacterAnim = false; break;
        case CharacterAnimOption::ANIM_4:  characterAnimationCtrl.set((Model_character)3,  true); enableCharacterAnim = false; break;
        case CharacterAnimOption::ANIM_5:  characterAnimationCtrl.set((Model_character)4,  true); enableCharacterAnim = false; break;
        case CharacterAnimOption::ANIM_6:  characterAnimationCtrl.set((Model_character)5,  true); enableCharacterAnim = false; break;
        case CharacterAnimOption::ANIM_7:  characterAnimationCtrl.set((Model_character)6,  true); enableCharacterAnim = false; break;
        case CharacterAnimOption::ANIM_8:  characterAnimationCtrl.set((Model_character)7,  true); enableCharacterAnim = false; break;
        case CharacterAnimOption::ANIM_9:  characterAnimationCtrl.set((Model_character)8,  true); enableCharacterAnim = false; break;
        case CharacterAnimOption::ANIM_10: characterAnimationCtrl.set((Model_character)9,  true); enableCharacterAnim = false; break;
        case CharacterAnimOption::ANIM_11: characterAnimationCtrl.set((Model_character)10, true); enableCharacterAnim = false; break;
        case CharacterAnimOption::ANIM_12: characterAnimationCtrl.set((Model_character)11, true); enableCharacterAnim = false; break;
        case CharacterAnimOption::ANIM_13: characterAnimationCtrl.set((Model_character)12, true); enableCharacterAnim = false; break;
        case CharacterAnimOption::ANIM_14: characterAnimationCtrl.set((Model_character)13, true); enableCharacterAnim = false; break;
        case CharacterAnimOption::ANIM_15: characterAnimationCtrl.set((Model_character)14, true); enableCharacterAnim = false; break;
        case CharacterAnimOption::ANIM_16: characterAnimationCtrl.set((Model_character)15, true); enableCharacterAnim = false; break;
        case CharacterAnimOption::ANIM_17: characterAnimationCtrl.set((Model_character)16, true); enableCharacterAnim = false; break;
        case CharacterAnimOption::ANIM_18: characterAnimationCtrl.set((Model_character)17, true); enableCharacterAnim = false; break;
        case CharacterAnimOption::ANIM_19: characterAnimationCtrl.set((Model_character)18, true); enableCharacterAnim = false; break;
        case CharacterAnimOption::ANIM_20: characterAnimationCtrl.set((Model_character)19, true); enableCharacterAnim = false; break;
        case CharacterAnimOption::ANIM_21: characterAnimationCtrl.set((Model_character)20, true); enableCharacterAnim = false; break;
        case CharacterAnimOption::ANIM_22: characterAnimationCtrl.set((Model_character)21, true); enableCharacterAnim = false; break;
        case CharacterAnimOption::ANIM_23: characterAnimationCtrl.set((Model_character)22, true); enableCharacterAnim = false; break;
        case CharacterAnimOption::ANIM_24: characterAnimationCtrl.set((Model_character)23, true); enableCharacterAnim = false; break;
        default:
            break;
    }

    *isActivePtr = false;
    characterAnimationCtrl.play();
    return selectedView;
}
