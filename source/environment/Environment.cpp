#include "Environment.h"
#include "core/globals.h"
#include <math.h>
#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

/**
 * @brief Converts a raw texture dimension in pixels to the corresponding
 *        libnds TEXTURE_SIZE_* enum value.
 *
 * @param size Texture width/height in pixels. Expected to be one of the
 *             power-of-two values 8, 16, 32, 64, 128, 256, 512, or 1024.
 * @return The matching TEXTURE_SIZE_* constant, or TEXTURE_SIZE_8 as a
 *         fallback if @p size does not match a supported value (a message
 *         is also printed to the debug console in that case).
 */
static int textureSizeEnum(int size)
{
    switch (size)
    {
    case 8:
        return TEXTURE_SIZE_8;
    case 16:
        return TEXTURE_SIZE_16;
    case 32:
        return TEXTURE_SIZE_32;
    case 64:
        return TEXTURE_SIZE_64;
    case 128:
        return TEXTURE_SIZE_128;
    case 256:
        return TEXTURE_SIZE_256;
    case 512:
        return TEXTURE_SIZE_512;
    case 1024:
        return TEXTURE_SIZE_1024;
    default:
        iprintf("Invalid texture size %d\n", size);
        return TEXTURE_SIZE_8;
    }
}

/**
 * @brief Constructs an empty Environment with all texture and display-list
 *        slots cleared and no database entry attached.
 */
Environment::Environment() : dbEntry(nullptr)
{
    for (int i = 0; i < MAX_ENVIRONMENT_TEXTURES; i++)
    {
        displayLists[i] = nullptr;
        dlSizes[i] = 0;
        textureIDs[i] = 0;
    }
}

/**
 * @brief Loads environment geometry (display lists) and textures from the
 *        compiled .bin file described by an EnvironmentDbEntry.
 *
 * Always begins with a hard reset (cleanup()) of any previously loaded
 * state, so this is safe to call repeatedly across room transitions
 * without leaking display lists or GPU texture slots. On any failure the
 * partially loaded state is cleaned up again before returning.
 *
 * @param entry   Pointer to the database entry describing this environment
 *                (name, texture metadata/count, binary file name). Must be
 *                non-null and have textureCount <= MAX_ENVIRONMENT_TEXTURES,
 *                otherwise this fails immediately.
 * @param bitmaps Array of raw bitmap pointers, one per texture slot, to be
 *                uploaded to VRAM. May be null, or contain null entries, in
 *                which case the corresponding texture slot(s) are skipped.
 * @return true if the binary file was opened, validated, and fully loaded
 *         (display lists and textures); false if @p entry was invalid, the
 *         file could not be opened or failed magic/size validation, or an
 *         allocation failed while reading display list data.
 *
 * @note entry->binaryFile is a build-time string baked in by
 *       obj2environment.py (e.g. "environments/iwatodai_dorm/iwatodai_dorm.bin").
 *       It cannot know the runtime fat/SD mount root, so - like every other
 *       file load in this codebase (textures, music, models) - it must be
 *       combined with fatBasePath before fopen() can find it.
 *       ASSUMPTION: binaryFile already contains the full
 *       "environments/<name>/" relative path, the same way it's referenced
 *       on the export side. If the printed path below is missing that
 *       folder, binaryFile is actually just the bare filename and this
 *       needs to be fatBasePath + "environments/" + entry->name + "/" +
 *       entry->binaryFile instead - check environmentDb.cpp's dorm entry to
 *       confirm which.
 */
bool Environment::load(const EnvironmentDbEntry* entry, const unsigned int* bitmaps[])
{
    // Hard safe entry reset - prevents cross-scene corruption.
    cleanup();

    // Guard against a missing/oversized db entry before touching it. A null
    // entry would otherwise crash on entry->binaryFile a few lines down; an
    // oversized one would silently overflow the fixed-size displayLists/
    // dlSizes/textureIDs arrays instead of failing cleanly.
    if (!entry || entry->textureCount > MAX_ENVIRONMENT_TEXTURES)
    {
        iprintf("EnvironmentDbEntry textures exceeds MAX_ENVIRONMENT_TEXTURES");
        return false;
    }

    dbEntry = entry;

    // See @note above re: fatBasePath + ASSUMPTION about binaryFile's path.
    const std::string fullBinaryPath = fatBasePath + "environments/" + entry->name + "/" + entry->binaryFile;

    if (Globals::enableDebugPrint)
    {
        iprintf("Environment::load opening '%s'\n", fullBinaryPath.c_str());
    }

    FILE* file = fopen(fullBinaryPath.c_str(), "rb");
    if (!file)
    {
        cleanup();
        return false;
    }

    char magic[4];
    if (fread(magic, 1, 4, file) != 4)
    {
        fclose(file);
        cleanup();
        return false;
    }

    if (magic[0] != 'E' || magic[1] != 'N' || magic[2] != 'V' || magic[3] != '1')
    {
        fclose(file);
        cleanup();
        return false;
    }

    u32 groupCount = 0;
    if (fread(&groupCount, sizeof(u32), 1, file) != 1)
    {
        fclose(file);
        cleanup();
        return false;
    }

    if (groupCount > (u32)entry->textureCount)
    {
        fclose(file);
        cleanup();
        return false;
    }

    /** @note Display list load - fully guarded against short reads and
     *        allocation failure. */
    for (u32 i = 0; i < groupCount; i++)
    {
        if (fread(&dlSizes[i], sizeof(u32), 1, file) != 1)
        {
            fclose(file);
            cleanup(); // frees any displayLists[0..i) already allocated this call
            return false;
        }

        displayLists[i] = nullptr;

        if (dlSizes[i] > 0)
        {
            displayLists[i] = (u32*)malloc((dlSizes[i] + 1) * sizeof(u32));

            if (!displayLists[i])
            {
                fclose(file);
                cleanup();
                return false;
            }

            displayLists[i][0] = dlSizes[i];

            if (fread(&displayLists[i][1], sizeof(u32), dlSizes[i], file) != dlSizes[i])
            {
                fclose(file);
                cleanup();
                return false;
            }
        }
    }

    fclose(file);

    /** @note Texture upload - safe binding only. */
    for (int i = 0; i < entry->textureCount; i++)
    {
        textureIDs[i] = 0;

        if (!bitmaps || !bitmaps[i])
            continue;

        glGenTextures(1, &textureIDs[i]);
        glBindTexture(GL_TEXTURE_2D, textureIDs[i]);

        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RGBA,
                     textureSizeEnum(entry->textures[i].width),
                     textureSizeEnum(entry->textures[i].height),
                     0,
                     TEXGEN_TEXCOORD | GL_TEXTURE_WRAP_S | GL_TEXTURE_WRAP_T,
                     bitmaps[i]);
    }

    return true;
}

/**
 * @brief Renders every loaded texture/display-list pair for the currently
 *        loaded environment.
 *
 * Does nothing if no environment is currently loaded (dbEntry is null).
 * Skips any texture slot that has no bound texture ID, and skips the draw
 * call itself (but still binds the texture) if that slot has no display
 * list, guarding against corrupted or missing display-list pointers.
 */
void Environment::draw()
{
    if (!dbEntry)
        return;

    for (int i = 0; i < dbEntry->textureCount; i++)
    {
        if (!textureIDs[i])
            continue;

        glBindTexture(GL_TEXTURE_2D, textureIDs[i]);

        if (displayLists[i])
        {
            // IMPORTANT: guard against corrupted DL pointers
            glCallList(displayLists[i]);
        }

        while (GFX_BUSY)
            ;
    }
}

/**
 * @brief Renders every billboard quad defined for the currently loaded
 *        environment, batching consecutive billboards that share a texture
 *        slot into a single glBegin/glEnd(GL_QUADS) block.
 *
 * Does nothing if no environment is loaded or it has no billboards. Any
 * billboard referencing an out-of-range or unbound texture slot is
 * skipped.
 *
 * @param faceCamera If true, each billboard's right vector is recomputed
 *                    every call so it always faces the given camera
 *                    position (classic screen-facing billboarding). If
 *                    false, billboards use a fixed world-axis-aligned
 *                    orientation.
 * @param camX World-space X coordinate of the camera, used only when
 *             @p faceCamera is true.
 * @param camY World-space Y coordinate of the camera (unused; billboards
 *             only rotate about the Y axis).
 * @param camZ World-space Z coordinate of the camera, used only when
 *             @p faceCamera is true.
 */
void Environment::drawBillboards(bool faceCamera, float camX, float camY, float camZ)
{
    if (!dbEntry || dbEntry->billboardCount == 0)
        return;

    int currentSlot = -1;
    bool inQuads = false;

    for (int i = 0; i < dbEntry->billboardCount; i++)
    {
        const auto& bb = dbEntry->billboards[i];

        if (bb.texSlot >= dbEntry->textureCount)
            continue;

        if (!textureIDs[bb.texSlot])
            continue;

        if (bb.texSlot != currentSlot)
        {
            if (inQuads)
            {
                glEnd();
                inQuads = false;
            }

            while (GFX_BUSY)
                ;

            glBindTexture(GL_TEXTURE_2D, textureIDs[bb.texSlot]);
            currentSlot = bb.texSlot;
        }

        if (!inQuads)
        {
            glBegin(GL_QUADS);
            inQuads = true;
        }

        v16 rX = 4096, rY = 0, rZ = 0;
        v16 uX = 0, uY = 4096, uZ = 0;

        if (faceCamera)
        {
            float bx = (float)bb.x / 4096.0f;
            float bz = (float)bb.z / 4096.0f;

            float dx = camX - bx;
            float dz = camZ - bz;

            float dist = sqrtf(dx * dx + dz * dz);

            if (dist > 0.001f)
            {
                dx /= dist;
                dz /= dist;
            }

            rX = (v16)(dz * 4096.0f);
            rZ = (v16)(-dx * 4096.0f);
        }

        v16 rx = mulf32(rX, bb.halfWidth);
        v16 ry = mulf32(rY, bb.halfWidth);
        v16 rz = mulf32(rZ, bb.halfWidth);

        v16 ux = mulf32(uX, bb.halfHeight);
        v16 uy = mulf32(uY, bb.halfHeight);
        v16 uz = mulf32(uZ, bb.halfHeight);

        glTexCoord2t16(bb.u0, bb.v1);
        glVertex3v16(bb.x - rx - ux, bb.y - ry - uy, bb.z - rz - uz);

        glTexCoord2t16(bb.u1, bb.v1);
        glVertex3v16(bb.x + rx - ux, bb.y + ry - uy, bb.z + rz - uz);

        glTexCoord2t16(bb.u1, bb.v0);
        glVertex3v16(bb.x + rx + ux, bb.y + ry + uy, bb.z + rz + uz);

        glTexCoord2t16(bb.u0, bb.v0);
        glVertex3v16(bb.x - rx + ux, bb.y - ry + uy, bb.z - rz + uz);
    }

    if (inQuads)
        glEnd();
}

/**
 * @brief Frees all display lists and deletes all GPU textures owned by
 *        this Environment, and clears the current database entry.
 *
 * Safe to call at any time, including when no environment is loaded (e.g.
 * from load()'s own failure paths, before dbEntry is set, or on the very
 * first load of a fresh Environment).
 *
 * @note Always sweeps the full fixed-size arrays rather than looping to
 *       dbEntry->textureCount. Looping to the "current" entry's count
 *       would mean a load() that failed partway through - or simply a room
 *       with fewer texture slots than the one loaded before it - could
 *       leave stale, un-freed pointers sitting past that count, silently
 *       leaking memory and VRAM texture slots across room transitions.
 */
void Environment::cleanup()
{
    for (int i = 0; i < MAX_ENVIRONMENT_TEXTURES; i++)
    {
        if (displayLists[i])
        {
            free(displayLists[i]);
            displayLists[i] = nullptr;
        }

        dlSizes[i] = 0;

        if (textureIDs[i])
        {
            // Previously this only zeroed the id without ever releasing the
            // underlying GPU texture slot, leaking VRAM texture memory on
            // every single room transition.
            glDeleteTextures(1, &textureIDs[i]);
            textureIDs[i] = 0;
        }
    }

    dbEntry = nullptr;
}
