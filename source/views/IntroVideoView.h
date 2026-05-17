#pragma once
#include "core/BaseView.h"
#include "core/globals.h"

class IntroVideoView : public BaseView
{
public:
    IntroVideoView(const char *filename) : filename(filename) {}
    void init() override;
    ViewState update() override;
    void cleanup() override;

private:
    const char *filename;
};
