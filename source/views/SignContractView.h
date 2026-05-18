#pragma once
#include "core/BaseView.h"
// #include "core/globals.h"

class SignContractView : public BaseView
{
public:
    void init() override;
    ViewState update() override;
    void cleanup() override;
};
