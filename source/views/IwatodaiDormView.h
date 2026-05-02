#pragma once
#include "core/View.h"
#include "controllers/DialogueController.h"
#include "controllers/CharacterController.h"
#include "maps/iwatodaiDorm_256x256_offsets.h"
#include <nds/arm9/console.h>
//TODO:  move somewhere
#include "./controllers/BattleController.h"


// implementing from View
class IwatodaiDormView : public View {
    public:
        // override tells compiler we intend to override a virtual fn in a base class (i.e. View)
        void Init() override;
        ViewState Update() override;
        void Cleanup() override;

    private:
        // sub screen
        // int bgAkihiko;
        PrintConsole console;

        // controllers
        BattleController battleController;
        CharacterController* playerCtrl;
            // camera pos
            cameraPosition camPos;
            // world
            // const float tileSize = 0.0625f;
            // const float worldOffsetX = 1.8125f;
            // const float worldOffsetZ = 1.6875f;
            const float tileSize = TILE_SIZE;
            const float worldOffsetX = WORLD_OFFSET_X;
            const float worldOffsetZ = WORLD_OFFSET_Z;
            const Point2D<float> characterSize = Point2D<float>(0.1f, 0.1f);
            // movement and viewpoint
            const float speed = 0.01f;
            const float angleIncrement = 0.05f;
            const float distance = 0.5f; 
            const float lookAhead = 0.3f;
            // set character initial translation position
            const Point2D<float> characterTranslate = Point2D<float>(-1.3, -0.8);
            const float angle = -1.6;
            const float characterFacingAngle = 91.67;
        DialogueController dialogueCtrl;
            dialogue lines[5];
};