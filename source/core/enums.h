#pragma once

enum SpriteType {
    NONE = 0,
    MOON,
    DAY_OF_WEEK,
    DIGIT,
    TIME,
    SKILL_SPRITE,
    CUSTOM,
};

enum MoonSprite       { MOON_0 = 0, MOON_1, MOON_2 };
enum DayOfWeekSprite  { TUESDAY = 0 };
enum TimeSprite       { AFTERNOON_0_0 = 0, AFTERNOON_1_0, AFTERNOON_2_0 };
enum SkillSprite      { SKILLS_LEVEL = 0 };
enum DigitSprite      { DIGIT_0 = 0, DIGIT_1, DIGIT_2, DIGIT_3, DIGIT_4,
                         DIGIT_5, DIGIT_6, DIGIT_7, DIGIT_8, DIGIT_9 };
