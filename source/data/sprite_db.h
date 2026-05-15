#include "core/enums.h"
#include "core/structs.h"
// grit-generated headers
// moons
#include "moon-0.h"
// days of the week
#include "tuesday.h"
// numbers
#include "number-4.h"
#include "number-7.h"
// time text
#include "afternnoon-0-0.h"
#include "afternnoon-1-0.h"
#include "afternnoon-2-0.h"
// skills
#include "skills-level.h"

// sprites sharing a palette should point at the same array
const SpriteDBEntry SPRITE_DB[] = {
    // type, id, tiles, tilesLen,  pal, palLen
    { MOON,         MOON_0,          moon_0Tiles,        moon_0TilesLen,        moon_0Pal,        moon_0PalLen },
    { DAY_OF_WEEK,  TUESDAY,         tuesdayTiles,       tuesdayTilesLen,       tuesdayPal,       tuesdayPalLen },
    { DIGIT,        DIGIT_4,         number_4Tiles,      number_4TilesLen,      number_4Pal,      number_4PalLen },
    { DIGIT,        DIGIT_7,         number_7Tiles,      number_7TilesLen,      number_7Pal,      number_7PalLen },
    { TIME,         AFTERNOON_0_0,   afternnoon_0_0Tiles,afternnoon_0_0TilesLen,afternnoon_0_0Pal,afternnoon_0_0PalLen },
    { TIME,         AFTERNOON_1_0,   afternnoon_1_0Tiles,afternnoon_1_0TilesLen,afternnoon_1_0Pal,afternnoon_1_0PalLen },
    { TIME,         AFTERNOON_2_0,   afternnoon_2_0Tiles,afternnoon_2_0TilesLen,afternnoon_2_0Pal,afternnoon_2_0PalLen },
    { SKILL_SPRITE, SKILLS_LEVEL,    skills_levelTiles,  skills_levelTilesLen,  skills_levelPal,  skills_levelPalLen },
};

const int SPRITE_DB_LEN = sizeof(SPRITE_DB) / sizeof(SPRITE_DB[0]);
