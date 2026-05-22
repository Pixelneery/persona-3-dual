#pragma once
#include "core/BaseView.h"
#include "core/globals.h"

class VideoView : public BaseView
{
public:
    VideoView(const char *filename, const ViewState nextView) : filename(filename), nextView(nextView) {}
    void init() override;
    ViewState update() override;
    void cleanup() override;

private:
    const char *filename;
    const ViewState nextView;
};
