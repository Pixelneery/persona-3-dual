#include <nds.h>
#include <stdio.h>
#include "core/globals.h"
#include "IntroVideoView.h"

void IntroVideoView::init()
{
    videoCtrl.init(filename, 15.0f, ViewState::INTRO, true);
}

ViewState IntroVideoView::update()
{
    return videoCtrl.update();
}

void IntroVideoView::cleanup()
{
    videoCtrl.cleanup();
}
