#include <nds.h>
#include <stdio.h>
// #include "core/globals.h"
#include "core/enums.h"
#include "SignContractView.h"

void SignContractView::init()
{
    // TODO: setup bg, music, sfx
    // TODO: setup properly (not using DemoInit)
    consoleDemoInit();
	keyboardDemoInit();
    keyboardShow();
}

ViewState SignContractView::update()
{
    int key = keyboardUpdate();

    if(key > 0)
    {
        iprintf("%c", key);
    }

    return ViewState::KEEP_CURRENT;
}

void SignContractView::cleanup()
{
    BaseView::cleanup();
}
