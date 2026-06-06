#pragma once
#include "core/enums.h"
#include "core/globals.h"
#include "core/structs.h"
#include <string>
#include <vector>

// Static storage for loaded sprite assets and database
static std::vector<GritAsset> spriteAssets;
static std::vector<SpriteDBEntry> SPRITE_DB;
static int SPRITE_DB_LEN = 0;

// Helper to load a sprite asset and add it to the database
static void addSpriteEntry(SpriteType type, int spriteId, const std::string& spriteName)
{
    GritAsset asset = graphicsCtrl.loadGrit(fatBasePath + "graphics/MenuHUD/sprites/" + spriteName);
    SpriteDBEntry entry;
    entry.type = type;
    entry.id = spriteId;
    entry.tiles = reinterpret_cast<const unsigned int*>(asset.tiles);
    entry.tilesLen = asset.tilesLen;
    entry.pal = reinterpret_cast<const unsigned short*>(asset.pal);
    entry.palLen = asset.palLen;

    spriteAssets.push_back(asset);
    SPRITE_DB.push_back(entry);
}

// Initialize sprite database at runtime
inline void initializeSpriteDatabase()
{
    if (SPRITE_DB.size() > 0)
        return; // Already initialized

    // Moon sprites (0-29)
    addSpriteEntry(SpriteType::MOON, static_cast<int>(MoonSprite::MOON_0), "moon-0");
    addSpriteEntry(SpriteType::MOON, static_cast<int>(MoonSprite::MOON_1), "moon-1");
    addSpriteEntry(SpriteType::MOON, static_cast<int>(MoonSprite::MOON_2), "moon-2");
    addSpriteEntry(SpriteType::MOON, static_cast<int>(MoonSprite::MOON_3), "moon-3");
    addSpriteEntry(SpriteType::MOON, static_cast<int>(MoonSprite::MOON_4), "moon-4");
    addSpriteEntry(SpriteType::MOON, static_cast<int>(MoonSprite::MOON_5), "moon-5");
    addSpriteEntry(SpriteType::MOON, static_cast<int>(MoonSprite::MOON_6), "moon-6");
    addSpriteEntry(SpriteType::MOON, static_cast<int>(MoonSprite::MOON_7), "moon-7");
    addSpriteEntry(SpriteType::MOON, static_cast<int>(MoonSprite::MOON_8), "moon-8");
    addSpriteEntry(SpriteType::MOON, static_cast<int>(MoonSprite::MOON_9), "moon-9");
    addSpriteEntry(SpriteType::MOON, static_cast<int>(MoonSprite::MOON_10), "moon-10");
    addSpriteEntry(SpriteType::MOON, static_cast<int>(MoonSprite::MOON_11), "moon-11");
    addSpriteEntry(SpriteType::MOON, static_cast<int>(MoonSprite::MOON_12), "moon-12");
    addSpriteEntry(SpriteType::MOON, static_cast<int>(MoonSprite::MOON_13), "moon-13");
    addSpriteEntry(SpriteType::MOON, static_cast<int>(MoonSprite::MOON_14), "moon-14");
    addSpriteEntry(SpriteType::MOON, static_cast<int>(MoonSprite::MOON_15), "moon-15");
    addSpriteEntry(SpriteType::MOON, static_cast<int>(MoonSprite::MOON_16), "moon-16");
    addSpriteEntry(SpriteType::MOON, static_cast<int>(MoonSprite::MOON_17), "moon-17");
    addSpriteEntry(SpriteType::MOON, static_cast<int>(MoonSprite::MOON_18), "moon-18");
    addSpriteEntry(SpriteType::MOON, static_cast<int>(MoonSprite::MOON_19), "moon-19");
    addSpriteEntry(SpriteType::MOON, static_cast<int>(MoonSprite::MOON_20), "moon-20");
    addSpriteEntry(SpriteType::MOON, static_cast<int>(MoonSprite::MOON_21), "moon-21");
    addSpriteEntry(SpriteType::MOON, static_cast<int>(MoonSprite::MOON_22), "moon-22");
    addSpriteEntry(SpriteType::MOON, static_cast<int>(MoonSprite::MOON_23), "moon-23");
    addSpriteEntry(SpriteType::MOON, static_cast<int>(MoonSprite::MOON_24), "moon-24");
    addSpriteEntry(SpriteType::MOON, static_cast<int>(MoonSprite::MOON_25), "moon-25");
    addSpriteEntry(SpriteType::MOON, static_cast<int>(MoonSprite::MOON_26), "moon-26");
    addSpriteEntry(SpriteType::MOON, static_cast<int>(MoonSprite::MOON_27), "moon-27");
    addSpriteEntry(SpriteType::MOON, static_cast<int>(MoonSprite::MOON_28), "moon-28");
    addSpriteEntry(SpriteType::MOON, static_cast<int>(MoonSprite::MOON_29), "moon-29");

    // Days of the week
    addSpriteEntry(SpriteType::DAY_OF_WEEK, static_cast<int>(DayOfWeekSprite::SUNDAY), "sunday");
    addSpriteEntry(SpriteType::DAY_OF_WEEK, static_cast<int>(DayOfWeekSprite::MONDAY), "monday");
    addSpriteEntry(SpriteType::DAY_OF_WEEK, static_cast<int>(DayOfWeekSprite::TUESDAY), "tuesday");
    addSpriteEntry(SpriteType::DAY_OF_WEEK, static_cast<int>(DayOfWeekSprite::WEDNESDAY), "wednesday");
    addSpriteEntry(SpriteType::DAY_OF_WEEK, static_cast<int>(DayOfWeekSprite::THURSDAY), "thursday");
    addSpriteEntry(SpriteType::DAY_OF_WEEK, static_cast<int>(DayOfWeekSprite::FRIDAY), "friday");
    addSpriteEntry(SpriteType::DAY_OF_WEEK, static_cast<int>(DayOfWeekSprite::SATURDAY), "saturday");

    // Digits 0-9
    addSpriteEntry(SpriteType::DIGIT, static_cast<int>(DigitSprite::DIGIT_0), "number-0");
    addSpriteEntry(SpriteType::DIGIT, static_cast<int>(DigitSprite::DIGIT_1), "number-1");
    addSpriteEntry(SpriteType::DIGIT, static_cast<int>(DigitSprite::DIGIT_2), "number-2");
    addSpriteEntry(SpriteType::DIGIT, static_cast<int>(DigitSprite::DIGIT_3), "number-3");
    addSpriteEntry(SpriteType::DIGIT, static_cast<int>(DigitSprite::DIGIT_4), "number-4");
    addSpriteEntry(SpriteType::DIGIT, static_cast<int>(DigitSprite::DIGIT_5), "number-5");
    addSpriteEntry(SpriteType::DIGIT, static_cast<int>(DigitSprite::DIGIT_6), "number-6");
    addSpriteEntry(SpriteType::DIGIT, static_cast<int>(DigitSprite::DIGIT_7), "number-7");
    addSpriteEntry(SpriteType::DIGIT, static_cast<int>(DigitSprite::DIGIT_8), "number-8");
    addSpriteEntry(SpriteType::DIGIT, static_cast<int>(DigitSprite::DIGIT_9), "number-9");

    // Time sprites
    addSpriteEntry(SpriteType::TIME, static_cast<int>(TimeSprite::AFTER_SCHOOL_0_0), "after-school-0-0");
    addSpriteEntry(SpriteType::TIME, static_cast<int>(TimeSprite::AFTER_SCHOOL_1_0), "after-school-1-0");
    addSpriteEntry(SpriteType::TIME, static_cast<int>(TimeSprite::AFTER_SCHOOL_2_0), "after-school-2-0");
    addSpriteEntry(SpriteType::TIME, static_cast<int>(TimeSprite::AFTERNOON_0_0), "afternoon-0-0");
    addSpriteEntry(SpriteType::TIME, static_cast<int>(TimeSprite::AFTERNOON_1_0), "afternoon-1-0");
    addSpriteEntry(SpriteType::TIME, static_cast<int>(TimeSprite::AFTERNOON_2_0), "afternoon-2-0");
    addSpriteEntry(SpriteType::TIME, static_cast<int>(TimeSprite::DAYTIME_0_0), "daytime-0-0");
    addSpriteEntry(SpriteType::TIME, static_cast<int>(TimeSprite::DAYTIME_1_0), "daytime-1-0");
    addSpriteEntry(SpriteType::TIME, static_cast<int>(TimeSprite::EARLY_MORNING_0_0), "early-morning-0-0");
    addSpriteEntry(SpriteType::TIME, static_cast<int>(TimeSprite::EARLY_MORNING_1_0), "early-morning-1-0");
    addSpriteEntry(SpriteType::TIME, static_cast<int>(TimeSprite::EARLY_MORNING_2_0), "early-morning-2-0");
    addSpriteEntry(SpriteType::TIME, static_cast<int>(TimeSprite::EARLY_MORNING_3_0), "early-morning-3-0");
    addSpriteEntry(SpriteType::TIME, static_cast<int>(TimeSprite::LATE_NIGHT_0_0), "late-night-0-0");
    addSpriteEntry(SpriteType::TIME, static_cast<int>(TimeSprite::LATE_NIGHT_1_0), "late-night-1-0");
    addSpriteEntry(SpriteType::TIME, static_cast<int>(TimeSprite::LATE_NIGHT_2_0), "late-night-2-0");
    addSpriteEntry(SpriteType::TIME, static_cast<int>(TimeSprite::LUNCHTIME_0_0), "lunchtime-0-0");
    addSpriteEntry(SpriteType::TIME, static_cast<int>(TimeSprite::LUNCHTIME_1_0), "lunchtime-1-0");
    addSpriteEntry(SpriteType::TIME, static_cast<int>(TimeSprite::LUNCHTIME_2_0), "lunchtime-2-0");
    addSpriteEntry(SpriteType::TIME, static_cast<int>(TimeSprite::MORNING_0_0), "morning-0-0");
    addSpriteEntry(SpriteType::TIME, static_cast<int>(TimeSprite::MORNING_1_0), "morning-1-0");

    // Skill sprite
    addSpriteEntry(SpriteType::SKILL_SPRITE, static_cast<int>(SkillSprite::SKILLS_LEVEL), "skills-level");

    // Slash
    addSpriteEntry(SpriteType::DIGIT, static_cast<int>(DigitSprite::SLASH), "slash");

    SPRITE_DB_LEN = SPRITE_DB.size();
}
