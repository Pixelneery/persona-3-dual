#pragma once
#include "core/enums.h"
#include <nds.h>
#include <vector>
#include <string>
class BaseMenu;

struct SpriteRegister
{
    int id;
    const unsigned int *tiles;
    unsigned int tilesLen;
    const unsigned short *pal;
    unsigned int palLen;
};

struct SpriteDBEntry
{
    SpriteType type;
    int id;
    const unsigned int *tiles;
    unsigned int tilesLen;
    const unsigned short *pal;
    unsigned int palLen;
};

//a simple sprite structure
struct Sprite
{
   u16* gfx;
   SpriteSize size;
   SpriteColorFormat format;
   int rotationIndex;
   int paletteAlpha;
   int x;
   int y;
};

struct MenuOption
{
    const char *name;
    int bgIndex;
    ViewState (BaseMenu::*onSelect)();
};

struct MenuState
{
    MenuOption *options;
    int optionCount;
    int selectedOption;
    int startIndex;
};

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
    float y;    // height
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

// Saves
struct Save
{
    std::string introVideoPath;
    std::string lastName;
    std::string firstName;
};
