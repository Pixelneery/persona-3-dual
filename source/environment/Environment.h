#pragma once
#include "controllers/GraphicsController.h"
#include <nds.h>

#include "data/environmentDb.h"

// Largest textureCount across all current g_environmentDb entries (dorm = 32).
// Bump this if a future room introduces more textures than that.
constexpr int MAX_ENVIRONMENT_TEXTURES = 32;

// One generic runtime representation of a room's 3D geometry, replacing what
// used to be a separate hand-generated *_Environment class per room
// (iwatodai_dorm_floor_1_Environment, iwatodai_streets_Environment, ...).
//
// Everything that used to vary between those generated classes - texture
// count, texture dimensions, billboard list, .bin filename, world bounds -
// now lives in a single EnvironmentDbEntry (data/environmentDb.cpp). This
// class just reads that data and does the same GPU work generically instead
// of once per room via unrolled per-slot code.
//
// obj2environment.py / obj2environment_modified.py no longer needs to emit a
// C++ header at all - only the compiled .bin file and the EnvironmentDbEntry
// block appended to environmentDb.cpp. source/environments/*.h (the old
// generated headers) can be deleted once every view has moved to this class.
class Environment
{
  public:
    Environment();

    // Reads the room's .bin display-list file (path derived from
    // entry->name / entry->binaryFile) and uploads each non-null bitmap to
    // VRAM, sized per entry->textures[i].width/height. Keeps a pointer to
    // `entry` for the lifetime of this Environment - draw()/drawBillboards()/
    // cleanup()/getPolyCount() all read counts and billboard data from it.
    bool load(const EnvironmentDbEntry* entry, const unsigned int* bitmaps[]);

    void draw();
    void drawBillboards(bool faceCamera, float camX, float camY, float camZ);
    void cleanup();
    int getPolyCount() const;

  private:
    const EnvironmentDbEntry* dbEntry;

    u32* displayLists[MAX_ENVIRONMENT_TEXTURES];
    u32 dlSizes[MAX_ENVIRONMENT_TEXTURES];
    int textureIDs[MAX_ENVIRONMENT_TEXTURES];
};
