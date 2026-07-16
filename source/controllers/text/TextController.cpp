#include "TextController.h"

#include "core/globals.h"
#include <fstream>
#include <sstream>

//TODO: Settings? i.e. video mode, vram bank, etc.
//TODO: Colors
//TODO: Fix Word wrapping
//TODO: Multiple font files & singleton solution?
//TODO: Wreorder warning

TextController::TextController(const std::string& fontFilePath)
    : fontBitmap(nullptr), fontBitmapWidth(0), fontBitmapHeight(0), fontLineHeight(0), fontPalette(nullptr)
{
    std::string fullPath = fatBasePath + "fonts/" + fontFilePath;

    vramSetBankE(VRAM_E_MAIN_BG);
    int bgID = bgInit(3, BgType_Bmp8, BgSize_B8_256x256, 0, 0);
    bgSetScale(bgID, 1 << 8, 1 << 8);
    videoBuffer = (uint16_t*)bgGetGfxPtr(bgID);
    bgSetPriority(bgID, 0);

    if (!loadFontBitmap(fullPath + ".img.bin"))
        haltOnError("Failed to load font bitmap from \n" + fullPath + ".img.bin");

    if (!loadFontPalette(fullPath + ".pal.bin"))
        haltOnError("Failed to load font palette from \n" + fullPath + ".pal.bin");

    // Load metadata to learn bitmap dimensions
    if (!loadFontMetadata(fullPath + ".fnt"))
        haltOnError("Failed to load font metadata from \n" + fullPath + ".fnt");
}

TextController::~TextController()
{
    //bgSetPriority(bgID, 3); //TODO: why dos this crash the game...on the second call
    bgSetScroll(bgID, 0, 0);
    bgSetPriority(0, 0);

    dmaFillHalfWords(0, videoBuffer, 256 * 256 * sizeof(uint16_t)); // Clear the video buffer
    dmaFillHalfWords(0, BG_PALETTE, 256 * sizeof(uint16_t));        // Clear the palette memory
    free(fontBitmap);
    free(fontPalette);
    //free(fontGlyphs);

    bgHide(bgID);
    videoBgDisable(bgID);
    vramSetBankE(VRAM_E_LCD);
}

// Loader Functions =====================================================

void* TextController::openFile(const std::string& path)
{
    FILE* file = fopen(path.c_str(), "rb");
    if (!file)
    {
        return nullptr;
    }

    //get file size
    fseek(file, 0, SEEK_END);
    u32 size = ftell(file);
    //return to beginning of file
    rewind(file);

    if (size == 0)
    {
        fclose(file);
        return nullptr;
    }

    void* buffer = malloc(size);
    if (buffer)
    {
        fread(buffer, 1, size, file);
    }
    fclose(file);

    return buffer;
}

void* TextController::openFile(const std::string& path, u32& size)
{
    FILE* file = fopen(path.c_str(), "rb");
    if (!file)
    {
        return nullptr;
    }

    //get file size
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    //return to beginning of file
    rewind(file);

    if (size == 0)
    {
        fclose(file);
        return nullptr;
    }

    void* buffer = malloc(size);
    if (buffer)
    {
        fread(buffer, 1, size, file);
    }
    fclose(file);

    return buffer;
}

bool TextController::loadFontBitmap(const std::string& path)
{
    void* buffer = openFile(path);
    if (buffer == nullptr)
    {
        return false;
    }

    fontBitmap = reinterpret_cast<std::uint8_t*>(buffer);

    return true;
}

bool TextController::loadFontPalette(const std::string& path)
{
    void* buffer = openFile(path);
    if (buffer == nullptr)
    {
        return false;
    }

    fontPalette = reinterpret_cast<std::uint16_t*>(buffer);
    dmaCopy(fontPalette, BG_PALETTE, 256 * sizeof(uint16_t)); // Copy the palette to the background palette memory
    return true;
}

bool TextController::loadFontMetadata(const std::string& path)
{
    u32 size;
    void* buffer = openFile(path, size);
    if (buffer == nullptr)
    {
        return false;
    }

    // create a string from the in-memory buffer and read it line-by-line
    std::string content(reinterpret_cast<char*>(buffer), size);
    free(buffer);

    std::istringstream iss(content);
    std::string line;
    while (std::getline(iss, line))
    {
        //Get the common data
        if (line.rfind("common ", 0) == 0)
        {
            fontLineHeight = extractIntValue(line, "lineHeight=");
            fontBitmapWidth = extractIntValue(line, "scaleW=");
            fontBitmapHeight = extractIntValue(line, "scaleH=");
            continue;
        }

        //If theres no character in this line move to the next
        if (line.rfind("char ", 0) != 0)
        {
            continue;
        }

        const int charId = extractIntValue(line, "id=");
        if (charId < 0 || charId >= 256)
        {
            continue;
        }

        //Create Glyph for the current char
        Glyph glyph{};
        glyph.xPos = extractIntValue(line, "x=");
        glyph.yPos = extractIntValue(line, "y=");
        glyph.width = extractIntValue(line, "width=");
        glyph.height = extractIntValue(line, "height=");
        glyph.xOffset = extractIntValue(line, "xoffset=");
        glyph.yOffset = extractIntValue(line, "yoffset=");
        fontGlyphs[charId] = glyph;
    }

    //Fallback values in case the font metadata is missing
    if (fontLineHeight <= 0)
    {
        fontLineHeight = 32;
    }

    if (fontBitmapWidth <= 0)
    {
        fontBitmapWidth = 256;
    }

    if (fontBitmapHeight <= 0)
    {
        fontBitmapHeight = 256;
    }

    //TODO: check if glyphs were loaded correctly?

    return true;
}

// Actual Drawing Functions =================================================

void TextController::drawText(const std::string& text, int startX, int startY, int color)
{
    int cursorX = startX;
    int cursorY = startY;

    for (char c : text)
    {
        //Handle Newline
        if (c == '\n')
        {
            cursorX = startX;
            cursorY += fontLineHeight + 2;
            continue;
        }

        Glyph g = fontGlyphs[static_cast<unsigned char>(c)];

        //Handle automatic word wrapping to prevent drawing outside the screen bounds
        if (c == ' ')
        {
            std::string nextWord = "";
            int i = 0;
            while (i < text.size() && text[i] != ' ' && text[i] != '\n')
            {
                nextWord += text[i];
                i++;
            }
            if (checkWordWrap(nextWord, cursorX))
            {
                cursorX = startX;
                cursorY += fontLineHeight + 2;
                continue;
            }
        }

        for (int y = 0; y < g.height; y++)
        {
            for (int x = 0; x < g.width; x++)
            {
                int srcX = g.xPos + x;
                int srcY = g.yPos + y;
                int pixelIndex = srcY * fontBitmapWidth + srcX;

                sassert(pixelIndex / 2 < fontBitmapWidth * fontBitmapHeight / 2,
                        "Pixel index out of bounds for font bitmap");

                int pixelValue = fontBitmap[pixelIndex];

                //If pixel is not black, draw it to screen (we're using 2 to try to filter out pixels that would be too dark to be seen)
                if (fontBitmap[pixelIndex] > 10 && pixelValue > 0)
                {
                    int screenX = cursorX + x;
                    int screenY = cursorY + g.yOffset + y;
                    //Only draw if within screen bounds otherwise disregard (from my experience this shouldn't crash the game, but text wrapping around would still be a bit of a problem)
                    if (screenX >= 0 && screenX < 256 && screenY >= 0 && screenY < 192)
                        drawPixel(screenX, screenY, pixelValue);
                }
            }
        }
        cursorX += g.width + 1;
    }
}

void TextController::clearScreen()
{
    dmaFillHalfWords(0, videoBuffer, 256 * 256 * sizeof(uint8_t)); // Clear the video buffer
}

// Helper Functions ======================================================

void TextController::drawPixel(int x, int y, int paletteIndex)
{
    int wordIndex = (y * 256 + x) / 2;
    u16 currentWord = videoBuffer[wordIndex];
    if (x % 2 == 0)
    {
        // Clear the lower 8 bits, then inject our 8-bit color index
        videoBuffer[wordIndex] = (currentWord & 0xFF00) | (fontPalette[paletteIndex] & 0xFF);
    }
    else
    {
        // Clear the upper 8 bits, then inject our 8-bit color index shifted up
        videoBuffer[wordIndex] = (currentWord & 0x00FF) | ((fontPalette[paletteIndex] & 0xFF) << 8);
    }
}

int TextController::extractIntValue(const std::string& line, const std::string& key)
{
    const std::size_t keyPos = line.find(key);
    if (keyPos == std::string::npos) //position not found
    {
        return 0;
    }

    const std::size_t dataStart = keyPos + key.size();
    std::size_t dataEnd = dataStart;
    while (dataEnd < line.size() && line[dataEnd] != ' ')
    {
        dataEnd++;
    }

    return std::stoi(line.substr(dataStart, dataEnd - dataStart));
}

bool TextController::checkWordWrap(const std::string& word, int startX)
{
    int cursorX = startX;

    for (char c : word)
    {
        Glyph g = fontGlyphs[static_cast<unsigned char>(c)];
        cursorX += g.width + g.xOffset;
    }
    if (cursorX > 256)
    {
        return true; // Word exceeds screen width
    }
    return false; // Word fits within screen width
}

void TextController::haltOnError(const std::string& errorMessage)
{
    consoleDemoInit();
    iprintf("Error: %s\n", errorMessage.c_str());
    while (1)
        swiWaitForVBlank();
}

// Test Functions ========================================================

void TextController::testBitmap()
{
    for (int y = 0; y < 256; y++)
    {
        for (int x = 0; x < 256; x++)
        {
            videoBuffer[y * 256 + x] = fontBitmap[y * fontBitmapWidth + x] | BIT(15);
        }
    }
}

void TextController::testPalette()
{
    for (int i = 0; i < (256 * 256); i++)
    {
        videoBuffer[i] = fontPalette[i % 256] | BIT(15);
    }
}
