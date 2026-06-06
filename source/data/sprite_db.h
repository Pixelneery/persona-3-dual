#pragma once

#include "core/structs.h"
#include <vector>

extern std::vector<SpriteDBEntry> SPRITE_DB;
extern int SPRITE_DB_LEN;

void initializeSpriteDatabase();
void unloadSpriteDatabase();
