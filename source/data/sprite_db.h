#include "core/enums.h"
#include "core/structs.h"
// grit-generated headers
// moons
#include "moon-0.h"
#include "moon-1.h"
#include "moon-2.h"
#include "moon-3.h"
#include "moon-4.h"
#include "moon-5.h"
#include "moon-6.h"
#include "moon-7.h"
#include "moon-8.h"
#include "moon-9.h"
#include "moon-10.h"
#include "moon-11.h"
#include "moon-12.h"
#include "moon-13.h"
#include "moon-14.h"
#include "moon-15.h"
#include "moon-16.h"
#include "moon-17.h"
#include "moon-18.h"
#include "moon-19.h"
#include "moon-20.h"
#include "moon-21.h"
#include "moon-22.h"
#include "moon-23.h"
#include "moon-24.h"
#include "moon-25.h"
#include "moon-26.h"
#include "moon-27.h"
#include "moon-28.h"
#include "moon-29.h"

// days of the week
#include "sunday.h"
#include "monday.h"
#include "tuesday.h"
#include "wednesday.h"
#include "thursday.h"
#include "friday.h"
#include "saturday.h"
// numbers
#include "number-0.h"
#include "number-1.h"
#include "number-2.h"
#include "number-3.h"
#include "number-4.h"
#include "number-5.h"
#include "number-6.h"
#include "number-7.h"
#include "number-8.h"
#include "number-9.h"
// time text
#include "afternnoon-0-0.h"
#include "afternnoon-1-0.h"
#include "afternnoon-2-0.h"
// skills
#include "skills-level.h"

// sprites sharing a palette should point at the same array
const SpriteDBEntry SPRITE_DB[] = {
    // type, id, tiles, tilesLen,  pal, palLen
    // moon
    {MOON, MOON_0, moon_0Tiles, moon_0TilesLen, moon_0Pal, moon_0PalLen},
    {MOON, MOON_1, moon_1Tiles, moon_1TilesLen, moon_1Pal, moon_1PalLen},
    {MOON, MOON_2, moon_2Tiles, moon_2TilesLen, moon_2Pal, moon_2PalLen},
    {MOON, MOON_3, moon_3Tiles, moon_3TilesLen, moon_3Pal, moon_3PalLen},
    {MOON, MOON_4, moon_4Tiles, moon_4TilesLen, moon_4Pal, moon_4PalLen},
    {MOON, MOON_5, moon_5Tiles, moon_5TilesLen, moon_5Pal, moon_5PalLen},
    {MOON, MOON_6, moon_6Tiles, moon_6TilesLen, moon_6Pal, moon_6PalLen},
    {MOON, MOON_7, moon_7Tiles, moon_7TilesLen, moon_7Pal, moon_7PalLen},
    {MOON, MOON_8, moon_8Tiles, moon_8TilesLen, moon_8Pal, moon_8PalLen},
    {MOON, MOON_9, moon_9Tiles, moon_9TilesLen, moon_9Pal, moon_9PalLen},
    {MOON, MOON_10, moon_10Tiles, moon_10TilesLen, moon_10Pal, moon_10PalLen},
    {MOON, MOON_11, moon_11Tiles, moon_11TilesLen, moon_11Pal, moon_11PalLen},
    {MOON, MOON_12, moon_12Tiles, moon_12TilesLen, moon_12Pal, moon_12PalLen},
    {MOON, MOON_13, moon_13Tiles, moon_13TilesLen, moon_13Pal, moon_13PalLen},
    {MOON, MOON_14, moon_14Tiles, moon_14TilesLen, moon_14Pal, moon_14PalLen},
    {MOON, MOON_15, moon_15Tiles, moon_15TilesLen, moon_15Pal, moon_15PalLen},
    {MOON, MOON_16, moon_16Tiles, moon_16TilesLen, moon_16Pal, moon_16PalLen},
    {MOON, MOON_17, moon_17Tiles, moon_17TilesLen, moon_17Pal, moon_17PalLen},
    {MOON, MOON_18, moon_18Tiles, moon_18TilesLen, moon_18Pal, moon_18PalLen},
    {MOON, MOON_19, moon_19Tiles, moon_19TilesLen, moon_19Pal, moon_19PalLen},
    {MOON, MOON_20, moon_20Tiles, moon_20TilesLen, moon_20Pal, moon_20PalLen},
    {MOON, MOON_21, moon_21Tiles, moon_21TilesLen, moon_21Pal, moon_21PalLen},
    {MOON, MOON_22, moon_22Tiles, moon_22TilesLen, moon_22Pal, moon_22PalLen},
    {MOON, MOON_23, moon_23Tiles, moon_23TilesLen, moon_23Pal, moon_23PalLen},
    {MOON, MOON_24, moon_24Tiles, moon_24TilesLen, moon_24Pal, moon_24PalLen},
    {MOON, MOON_25, moon_25Tiles, moon_25TilesLen, moon_25Pal, moon_25PalLen},
    {MOON, MOON_26, moon_26Tiles, moon_26TilesLen, moon_26Pal, moon_26PalLen},
    {MOON, MOON_27, moon_27Tiles, moon_27TilesLen, moon_27Pal, moon_27PalLen},
    {MOON, MOON_28, moon_28Tiles, moon_28TilesLen, moon_28Pal, moon_28PalLen},
    {MOON, MOON_29, moon_29Tiles, moon_29TilesLen, moon_29Pal, moon_29PalLen},

    // days of thw week
    {DAY_OF_WEEK, SUNDAY, sundayTiles, sundayTilesLen, sundayPal, sundayPalLen},
    {DAY_OF_WEEK, MONDAY, mondayTiles, mondayTilesLen, mondayPal, mondayPalLen},
    {DAY_OF_WEEK, TUESDAY, tuesdayTiles, tuesdayTilesLen, tuesdayPal, tuesdayPalLen},
    {DAY_OF_WEEK, WEDNESDAY, wednesdayTiles, wednesdayTilesLen, wednesdayPal, wednesdayPalLen},
    {DAY_OF_WEEK,
     THURSDAY, thursdayTiles, thursdayTilesLen, thursdayPal, thursdayPalLen},
    {DAY_OF_WEEK, FRIDAY, fridayTiles, fridayTilesLen, fridayPal, fridayPalLen},
    {DAY_OF_WEEK, SATURDAY, saturdayTiles, saturdayTilesLen, saturdayPal, saturdayPalLen},

    // digits
    {DIGIT, DIGIT_0, number_0Tiles, number_0TilesLen, number_0Pal, number_0PalLen},
    {DIGIT, DIGIT_1, number_1Tiles, number_1TilesLen, number_1Pal, number_1PalLen},
    {DIGIT, DIGIT_2, number_2Tiles, number_2TilesLen, number_2Pal, number_2PalLen},
    {DIGIT, DIGIT_3, number_3Tiles, number_3TilesLen, number_3Pal, number_3PalLen},
    {DIGIT, DIGIT_4, number_4Tiles, number_4TilesLen, number_4Pal, number_4PalLen},
    {DIGIT, DIGIT_5, number_5Tiles, number_5TilesLen, number_5Pal, number_5PalLen},
    {DIGIT, DIGIT_6, number_6Tiles, number_6TilesLen, number_6Pal, number_6PalLen},
    {DIGIT, DIGIT_7, number_7Tiles, number_7TilesLen, number_7Pal, number_7PalLen},
    {DIGIT, DIGIT_8, number_8Tiles, number_8TilesLen, number_8Pal, number_8PalLen},
    {DIGIT, DIGIT_9, number_9Tiles, number_9TilesLen, number_9Pal, number_9PalLen},

    // time
    {TIME, AFTERNOON_0_0, afternnoon_0_0Tiles, afternnoon_0_0TilesLen, afternnoon_0_0Pal, afternnoon_0_0PalLen},
    {TIME, AFTERNOON_1_0, afternnoon_1_0Tiles, afternnoon_1_0TilesLen, afternnoon_1_0Pal, afternnoon_1_0PalLen},
    {TIME, AFTERNOON_2_0, afternnoon_2_0Tiles, afternnoon_2_0TilesLen, afternnoon_2_0Pal, afternnoon_2_0PalLen},

    // skills
    {SKILL_SPRITE, SKILLS_LEVEL, skills_levelTiles, skills_levelTilesLen, skills_levelPal, skills_levelPalLen},
};

const int SPRITE_DB_LEN = sizeof(SPRITE_DB) / sizeof(SPRITE_DB[0]);
