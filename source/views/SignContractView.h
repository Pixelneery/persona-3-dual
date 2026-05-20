#pragma once
#include "core/BaseView.h"

class SignContractView : public BaseView
{
private:
    int bg[3];
    bool isLastName = true;
    bool isNameConfirmed = false;

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
