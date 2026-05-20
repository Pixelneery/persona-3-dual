#pragma once
#include "core/BaseView.h"
// #include "core/globals.h"

class SignContractView : public BaseView
{
private:
    int bg[3];
    // sfx
    mm_sfxhand sfxMenuHandle;
    mm_sfxhand sfxSelectHandle;
    mm_sfxhand sfxCancelHandle;

    void cancelSFX();
public:
    void init() override;
    ViewState update() override;
    void cleanup() override;
};
