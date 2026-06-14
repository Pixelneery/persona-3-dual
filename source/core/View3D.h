#pragma once
#include "core/BaseView.h"
#include "core/globals.h"
#include <nds.h>

class View3D : public BaseView
{
  public:
    virtual void init() override;
    virtual void setupEnvironment() = 0;
    // update needs to be overridden
    // cleanup needs to be overridden
};
