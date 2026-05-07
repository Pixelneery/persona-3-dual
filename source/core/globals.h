#pragma once
#include "controllers/MusicController.h"
#include "controllers/VideoController.h"
#include "controllers/AnimationController.h"

extern volatile int frame;
extern volatile bool enableBillboards;
extern int fps;
extern int fpsTimer;
extern MusicController musicCtrl;
extern VideoController videoCtrl;
extern AnimationController characterAnimationCtrl;