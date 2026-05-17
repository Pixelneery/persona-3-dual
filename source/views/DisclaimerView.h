#pragma once
#include <nds.h>
#include "core/BaseView.h"
#include "core/globals.h"

class DisclaimerView : public BaseView
{
private:
    int bg[2];

public:
    // override tells compiler we intend to override a virtual fn in a base class (i.e. View)
    void init() override;
    ViewState update() override;
    void cleanup() override;
};
