#include <nds.h>
#include <stdio.h>
#include "core/globals.h"
#include "VideoView.h"

void VideoView::init()
{
    videoCtrl.init(filename, 15.0f, nextView, true);
}

ViewState VideoView::update()
{
    return videoCtrl.update();
}

void VideoView::cleanup()
{
    // handles videoCtrl.cleanup()
    BaseView::cleanup();
}
