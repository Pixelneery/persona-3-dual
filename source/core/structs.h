#pragma once
#include "core/enums.h"
#include <nds.h>
#include <vector>
#include <string>
class BaseMenu;

typedef struct
{
    int id;
    const unsigned int *tiles;
    unsigned int tilesLen;
    const unsigned short *pal;
    unsigned int palLen;
} SpriteRegister;

typedef struct
{
    SpriteType type;
    int id;
    const unsigned int *tiles;
    unsigned int tilesLen;
    const unsigned short *pal;
    unsigned int palLen;
} SpriteDBEntry;

//a simple sprite structure
typedef struct
{
   u16* gfx;
   SpriteSize size;
   SpriteColorFormat format;
   int rotationIndex;
   int paletteAlpha;
   int x;
   int y;
} Sprite;

typedef struct
{
    const char *name;
    int bgIndex;
    ViewState (BaseMenu::*onSelect)();
} MenuOption;

typedef struct
{
    MenuOption *options;
    int optionCount;
    int selectedOption;
    int startIndex;
} MenuState;

// From AnimationController.h
struct Keyframe
{
    int time;
    s16 rotX, rotY, rotZ;
    s16 posX, posY, posZ;
};

struct AnimTrack
{
    std::vector<Keyframe> frames;
};

struct Animation
{
    int duration;
    std::vector<AnimTrack> nodeTracks;
};

struct SubList
{
    int texSlot;
    std::vector<u32> displayList;
    u32 displayListSize;
};

struct AnimNode
{
    int id;
    int parentId;
    std::vector<SubList> subLists;
    std::vector<int> children;
    v16 pivotX, pivotY, pivotZ;
};

// From CharacterController.h
struct cameraPosition
{
    float cameraX;
    float cameraY;
    float cameraZ;
    float targetX;
    float targetY;
    float targetZ;
    float upX;
    float upY;
    float upZ;
};

struct characterPosition
{
    float x;
    float z;
    float angle;
    float facingAngle;
};

// From DialogueController.h
struct dialogue;
struct dialogueSelection
{
    std::string text;
    bool isSelected;
    dialogue *next;
};
struct dialogue
{
    std::string characterName;
    std::string text;
    int imageId;
    dialogue *prev;
    dialogue *next;
    std::vector<dialogueSelection> selections;
};
