typedef struct
{
    int id;
    int tiles;
    int tilesLen;
    int pal;
    int palLen;
    bool operator<(const SpriteRegister &other) const { return id < other.id; }
} SpriteRegister;
