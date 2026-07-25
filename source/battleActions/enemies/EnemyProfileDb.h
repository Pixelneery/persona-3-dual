#pragma once
#include "EnemyProfile.h"

struct EnemyProfileDb
{
    static EnemyProfile mercilessMaya;
    static EnemyProfile cowardlyMaya;

    static void Initialize();

  private:
    static Skill* mercilessMayaSkills[1];
    static Skill* cowardlyMayaSkills[1];
};
