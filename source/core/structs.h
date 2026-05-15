#pragma once
typedef struct {
    int id;
    const unsigned int*   tiles;
    unsigned int          tilesLen;
    const unsigned short* pal;
    unsigned int          palLen;
} SpriteRegister;

typedef struct {
    int                   type;   // cast to SpriteType when registering
    int                   id;
    const unsigned int*   tiles;
    unsigned int          tilesLen;
    const unsigned short* pal;
    unsigned int          palLen;
} SpriteDBEntry;
