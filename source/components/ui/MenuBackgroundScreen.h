#pragma once

#include <nds.h>

#include "components/ui/UIScreen.h"
#include "controllers/GraphicsController.h"
#include "core/structs.h"

class MenuBackgroundScreen : public UIScreen
{
  public:
    static void create();
    static void destroy();
    static MenuBackgroundScreen* getInstance();

  private:
    MenuBackgroundScreen() {};
    ~MenuBackgroundScreen() {};

    static MenuBackgroundScreen* instance;
};
