#include "BaseView.h"
#include "core/globals.h"

void BaseView::cleanup()
{
    // clear screen
    setBrightness(3, 0);
    consoleClear();

    // global controllers
    videoCtrl.cleanup();
    musicCtrl.cleanup();
    characterAnimationCtrl.stop();

    // global components
    pauseMenuCmpt.cancelSFX();
    enableBillboards = true;
    enableCharacterAnim = true;
    enableDebugPrint = false;
    isPauseMenuActive = false;

    // reset vram
    vramSetPrimaryBanks(VRAM_A_LCD, VRAM_B_LCD, VRAM_C_LCD, VRAM_D_LCD);
    vramSetBanks_EFG(VRAM_E_LCD, VRAM_F_LCD, VRAM_G_LCD);
    vramSetBankH(VRAM_H_LCD);
    vramSetBankI(VRAM_I_LCD);
}
