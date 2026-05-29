#include "core/enums.h"
#include "core/structs.h"
// grit-generated headers
// moons
#include "moon-0.h"
#include "moon-1.h"
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
#include "moon-2.h"
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
#include "moon-3.h"
#include "moon-4.h"
#include "moon-5.h"
#include "moon-6.h"
#include "moon-7.h"
#include "moon-8.h"
#include "moon-9.h"

// days of the week
#include "friday.h"
#include "monday.h"
#include "saturday.h"
#include "sunday.h"
#include "thursday.h"
#include "tuesday.h"
#include "wednesday.h"
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
#include "after-school-0-0.h"
#include "after-school-1-0.h"
#include "after-school-2-0.h"
#include "afternoon-0-0.h"
#include "afternoon-1-0.h"
#include "afternoon-2-0.h"
#include "daytime-0-0.h"
#include "daytime-1-0.h"
#include "early-morning-0-0.h"
#include "early-morning-1-0.h"
#include "early-morning-2-0.h"
#include "early-morning-3-0.h"
#include "late-night-0-0.h"
#include "late-night-1-0.h"
#include "late-night-2-0.h"
#include "lunchtime-0-0.h"
#include "lunchtime-1-0.h"
#include "lunchtime-2-0.h"
#include "morning-0-0.h"
#include "morning-1-0.h"
// skills
#include "skills-level.h"
// slash (w/ digits)
#include "slash.h"

// sprites sharing a palette should point at the same array
const SpriteDBEntry SPRITE_DB[] = {
    // type, id, tiles, tilesLen,  pal, palLen
    // moon
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_0), moon_0Tiles, moon_0TilesLen, moon_0Pal, moon_0PalLen},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_1), moon_1Tiles, moon_1TilesLen, moon_1Pal, moon_1PalLen},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_2), moon_2Tiles, moon_2TilesLen, moon_2Pal, moon_2PalLen},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_3), moon_3Tiles, moon_3TilesLen, moon_3Pal, moon_3PalLen},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_4), moon_4Tiles, moon_4TilesLen, moon_4Pal, moon_4PalLen},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_5), moon_5Tiles, moon_5TilesLen, moon_5Pal, moon_5PalLen},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_6), moon_6Tiles, moon_6TilesLen, moon_6Pal, moon_6PalLen},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_7), moon_7Tiles, moon_7TilesLen, moon_7Pal, moon_7PalLen},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_8), moon_8Tiles, moon_8TilesLen, moon_8Pal, moon_8PalLen},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_9), moon_9Tiles, moon_9TilesLen, moon_9Pal, moon_9PalLen},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_10), moon_10Tiles, moon_10TilesLen, moon_10Pal, moon_10PalLen},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_11), moon_11Tiles, moon_11TilesLen, moon_11Pal, moon_11PalLen},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_12), moon_12Tiles, moon_12TilesLen, moon_12Pal, moon_12PalLen},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_13), moon_13Tiles, moon_13TilesLen, moon_13Pal, moon_13PalLen},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_14), moon_14Tiles, moon_14TilesLen, moon_14Pal, moon_14PalLen},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_15), moon_15Tiles, moon_15TilesLen, moon_15Pal, moon_15PalLen},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_16), moon_16Tiles, moon_16TilesLen, moon_16Pal, moon_16PalLen},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_17), moon_17Tiles, moon_17TilesLen, moon_17Pal, moon_17PalLen},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_18), moon_18Tiles, moon_18TilesLen, moon_18Pal, moon_18PalLen},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_19), moon_19Tiles, moon_19TilesLen, moon_19Pal, moon_19PalLen},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_20), moon_20Tiles, moon_20TilesLen, moon_20Pal, moon_20PalLen},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_21), moon_21Tiles, moon_21TilesLen, moon_21Pal, moon_21PalLen},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_22), moon_22Tiles, moon_22TilesLen, moon_22Pal, moon_22PalLen},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_23), moon_23Tiles, moon_23TilesLen, moon_23Pal, moon_23PalLen},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_24), moon_24Tiles, moon_24TilesLen, moon_24Pal, moon_24PalLen},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_25), moon_25Tiles, moon_25TilesLen, moon_25Pal, moon_25PalLen},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_26), moon_26Tiles, moon_26TilesLen, moon_26Pal, moon_26PalLen},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_27), moon_27Tiles, moon_27TilesLen, moon_27Pal, moon_27PalLen},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_28), moon_28Tiles, moon_28TilesLen, moon_28Pal, moon_28PalLen},
    {SpriteType::MOON, static_cast<int>(MoonSprite::MOON_29), moon_29Tiles, moon_29TilesLen, moon_29Pal, moon_29PalLen},

    // days of the week
    {SpriteType::DAY_OF_WEEK,
     static_cast<int>(DayOfWeekSprite::SUNDAY),
     sundayTiles,
     sundayTilesLen,
     sundayPal,
     sundayPalLen},
    {SpriteType::DAY_OF_WEEK,
     static_cast<int>(DayOfWeekSprite::MONDAY),
     mondayTiles,
     mondayTilesLen,
     mondayPal,
     mondayPalLen},
    {SpriteType::DAY_OF_WEEK,
     static_cast<int>(DayOfWeekSprite::TUESDAY),
     tuesdayTiles,
     tuesdayTilesLen,
     tuesdayPal,
     tuesdayPalLen},
    {SpriteType::DAY_OF_WEEK,
     static_cast<int>(DayOfWeekSprite::WEDNESDAY),
     wednesdayTiles,
     wednesdayTilesLen,
     wednesdayPal,
     wednesdayPalLen},
    {SpriteType::DAY_OF_WEEK,
     static_cast<int>(DayOfWeekSprite::THURSDAY),
     thursdayTiles,
     thursdayTilesLen,
     thursdayPal,
     thursdayPalLen},
    {SpriteType::DAY_OF_WEEK,
     static_cast<int>(DayOfWeekSprite::FRIDAY),
     fridayTiles,
     fridayTilesLen,
     fridayPal,
     fridayPalLen},
    {SpriteType::DAY_OF_WEEK,
     static_cast<int>(DayOfWeekSprite::SATURDAY),
     saturdayTiles,
     saturdayTilesLen,
     saturdayPal,
     saturdayPalLen},

    // digits
    {SpriteType::DIGIT,
     static_cast<int>(DigitSprite::DIGIT_0),
     number_0Tiles,
     number_0TilesLen,
     number_0Pal,
     number_0PalLen},
    {SpriteType::DIGIT,
     static_cast<int>(DigitSprite::DIGIT_1),
     number_1Tiles,
     number_1TilesLen,
     number_1Pal,
     number_1PalLen},
    {SpriteType::DIGIT,
     static_cast<int>(DigitSprite::DIGIT_2),
     number_2Tiles,
     number_2TilesLen,
     number_2Pal,
     number_2PalLen},
    {SpriteType::DIGIT,
     static_cast<int>(DigitSprite::DIGIT_3),
     number_3Tiles,
     number_3TilesLen,
     number_3Pal,
     number_3PalLen},
    {SpriteType::DIGIT,
     static_cast<int>(DigitSprite::DIGIT_4),
     number_4Tiles,
     number_4TilesLen,
     number_4Pal,
     number_4PalLen},
    {SpriteType::DIGIT,
     static_cast<int>(DigitSprite::DIGIT_5),
     number_5Tiles,
     number_5TilesLen,
     number_5Pal,
     number_5PalLen},
    {SpriteType::DIGIT,
     static_cast<int>(DigitSprite::DIGIT_6),
     number_6Tiles,
     number_6TilesLen,
     number_6Pal,
     number_6PalLen},
    {SpriteType::DIGIT,
     static_cast<int>(DigitSprite::DIGIT_7),
     number_7Tiles,
     number_7TilesLen,
     number_7Pal,
     number_7PalLen},
    {SpriteType::DIGIT,
     static_cast<int>(DigitSprite::DIGIT_8),
     number_8Tiles,
     number_8TilesLen,
     number_8Pal,
     number_8PalLen},
    {SpriteType::DIGIT,
     static_cast<int>(DigitSprite::DIGIT_9),
     number_9Tiles,
     number_9TilesLen,
     number_9Pal,
     number_9PalLen},

    // time
    {SpriteType::TIME,
     static_cast<int>(TimeSprite::AFTER_SCHOOL_0_0),
     after_school_0_0Tiles,
     after_school_0_0TilesLen,
     after_school_0_0Pal,
     after_school_0_0PalLen},
    {SpriteType::TIME,
     static_cast<int>(TimeSprite::AFTER_SCHOOL_1_0),
     after_school_1_0Tiles,
     after_school_1_0TilesLen,
     after_school_1_0Pal,
     after_school_1_0PalLen},
    {SpriteType::TIME,
     static_cast<int>(TimeSprite::AFTER_SCHOOL_2_0),
     after_school_2_0Tiles,
     after_school_2_0TilesLen,
     after_school_2_0Pal,
     after_school_2_0PalLen},
    {SpriteType::TIME,
     static_cast<int>(TimeSprite::AFTERNOON_0_0),
     afternoon_0_0Tiles,
     afternoon_0_0TilesLen,
     afternoon_0_0Pal,
     afternoon_0_0PalLen},
    {SpriteType::TIME,
     static_cast<int>(TimeSprite::AFTERNOON_1_0),
     afternoon_1_0Tiles,
     afternoon_1_0TilesLen,
     afternoon_1_0Pal,
     afternoon_1_0PalLen},
    {SpriteType::TIME,
     static_cast<int>(TimeSprite::AFTERNOON_2_0),
     afternoon_2_0Tiles,
     afternoon_2_0TilesLen,
     afternoon_2_0Pal,
     afternoon_2_0PalLen},
    {SpriteType::TIME,
     static_cast<int>(TimeSprite::DAYTIME_0_0),
     daytime_0_0Tiles,
     daytime_0_0TilesLen,
     daytime_0_0Pal,
     daytime_0_0PalLen},
    {SpriteType::TIME,
     static_cast<int>(TimeSprite::DAYTIME_1_0),
     daytime_1_0Tiles,
     daytime_1_0TilesLen,
     daytime_1_0Pal,
     daytime_1_0PalLen},
    {SpriteType::TIME,
     static_cast<int>(TimeSprite::EARLY_MORNING_0_0),
     early_morning_0_0Tiles,
     early_morning_0_0TilesLen,
     early_morning_0_0Pal,
     early_morning_0_0PalLen},
    {SpriteType::TIME,
     static_cast<int>(TimeSprite::EARLY_MORNING_1_0),
     early_morning_1_0Tiles,
     early_morning_1_0TilesLen,
     early_morning_1_0Pal,
     early_morning_1_0PalLen},
    {SpriteType::TIME,
     static_cast<int>(TimeSprite::EARLY_MORNING_2_0),
     early_morning_2_0Tiles,
     early_morning_2_0TilesLen,
     early_morning_2_0Pal,
     early_morning_2_0PalLen},
    {SpriteType::TIME,
     static_cast<int>(TimeSprite::EARLY_MORNING_3_0),
     early_morning_3_0Tiles,
     early_morning_3_0TilesLen,
     early_morning_3_0Pal,
     early_morning_3_0PalLen},
    {SpriteType::TIME,
     static_cast<int>(TimeSprite::LATE_NIGHT_0_0),
     late_night_0_0Tiles,
     late_night_0_0TilesLen,
     late_night_0_0Pal,
     late_night_0_0PalLen},
    {SpriteType::TIME,
     static_cast<int>(TimeSprite::LATE_NIGHT_1_0),
     late_night_1_0Tiles,
     late_night_1_0TilesLen,
     late_night_1_0Pal,
     late_night_1_0PalLen},
    {SpriteType::TIME,
     static_cast<int>(TimeSprite::LATE_NIGHT_2_0),
     late_night_2_0Tiles,
     late_night_2_0TilesLen,
     late_night_2_0Pal,
     late_night_2_0PalLen},
    {SpriteType::TIME,
     static_cast<int>(TimeSprite::LUNCHTIME_0_0),
     lunchtime_0_0Tiles,
     lunchtime_0_0TilesLen,
     lunchtime_0_0Pal,
     lunchtime_0_0PalLen},
    {SpriteType::TIME,
     static_cast<int>(TimeSprite::LUNCHTIME_1_0),
     lunchtime_1_0Tiles,
     lunchtime_1_0TilesLen,
     lunchtime_1_0Pal,
     lunchtime_1_0PalLen},
    {SpriteType::TIME,
     static_cast<int>(TimeSprite::LUNCHTIME_2_0),
     lunchtime_2_0Tiles,
     lunchtime_2_0TilesLen,
     lunchtime_2_0Pal,
     lunchtime_2_0PalLen},
    {SpriteType::TIME,
     static_cast<int>(TimeSprite::MORNING_0_0),
     morning_0_0Tiles,
     morning_0_0TilesLen,
     morning_0_0Pal,
     morning_0_0PalLen},
    {SpriteType::TIME,
     static_cast<int>(TimeSprite::MORNING_1_0),
     morning_1_0Tiles,
     morning_1_0TilesLen,
     morning_1_0Pal,
     morning_1_0PalLen},

    // skills
    {SpriteType::SKILL_SPRITE,
     static_cast<int>(SkillSprite::SKILLS_LEVEL),
     skills_levelTiles,
     skills_levelTilesLen,
     skills_levelPal,
     skills_levelPalLen},

    // slash (w/ digits)
    {SpriteType::DIGIT, static_cast<int>(DigitSprite::SLASH), slashTiles, slashTilesLen, slashPal, slashPalLen},
};

const int SPRITE_DB_LEN = sizeof(SPRITE_DB) / sizeof(SPRITE_DB[0]);
