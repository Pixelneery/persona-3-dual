#pragma once
#include "core/View.h"
#include "core/globals.h"

// implementing from View
class IntroVideoView : public View
{
public:
    IntroVideoView(const char *filename) : filename(filename) {}
    void Init() override;
    ViewState Update() override;
    void Cleanup() override;

private:
    const char *filename;
};
