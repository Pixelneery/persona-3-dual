#pragma once
#include "controllers/MusicController.h"
#include "controllers/VideoController.h"
#include "controllers/AnimationController.h"

extern volatile int frame;
extern MusicController musicCtrl;
extern VideoController videoCtrl;
extern AnimationController characterAnimationCtrl;