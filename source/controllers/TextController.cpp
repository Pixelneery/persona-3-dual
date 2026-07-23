#include "TextController.h"

#include "core/globals.h"
#include <fstream>
#include <sstream>

static const uint16_t customPalette[256] = {
    ARGB16(1, 0, 0, 0),    // Transparent
    ARGB16(1, 0, 0, 0),    // Black
    ARGB16(1, 31, 31, 31), // White
    ARGB16(1, 31, 0, 0),   // Red
    ARGB16(1, 0, 31, 0),   // Green
    ARGB16(1, 0, 0, 31),   // Blue
    ARGB16(1, 31, 31, 0),  // Yellow
    ARGB16(1, 31, 0, 31),  // Magenta
    ARGB16(1, 0, 31, 31),  // Cyan
    ARGB16(1, 15, 15, 15), // Gray
};

TextController::TextController()
{
    dmaCopy(customPalette, BG_PALETTE, 256 * sizeof(uint16_t));
    dmaCopy(customPalette, BG_PALETTE_SUB, 256 * sizeof(uint16_t));
}

// Loading Functions =====================================================

Font* TextController::loadFont(const std::string& fontFilePath)
{
    Font* font = new Font();
    std::string fullPath = fatBasePath + "fonts/" + fontFilePath;
    font->bitmap = loadFontBitmap(fullPath + ".img.bin");
    if (!font->bitmap)
        haltOnError("Failed to load font bitmap from \n" + fullPath + ".img.bin");
    if (!loadFontMetadata(fullPath + ".fnt", font))
        haltOnError("Failed to load font metadata from \n" + fullPath + ".fnt");
    return font;
}

void* TextController::openFile(const std::string& path)
{
    FILE* file = fopen(path.c_str(), "rb");
    if (!file)
        return nullptr;

    //get file size
    fseek(file, 0, SEEK_END);
    u32 size = ftell(file);
    rewind(file);

    if (size == 0)
    {
        fclose(file);
        return nullptr;
    }

    void* buffer = malloc(size);
    if (buffer)
        fread(buffer, 1, size, file);
    fclose(file);

    return buffer;
}

void* TextController::openFile(const std::string& path, u32& size)
{
    FILE* file = fopen(path.c_str(), "rb");
    if (!file)
        return nullptr;

    fseek(file, 0, SEEK_END);
    size = ftell(file);
    rewind(file);

    if (size == 0)
    {
        fclose(file);
        return nullptr;
    }

    void* buffer = malloc(size);
    if (buffer)
        fread(buffer, 1, size, file);
    fclose(file);

    return buffer;
}

std::uint8_t* TextController::loadFontBitmap(const std::string& path)
{
    void* buffer = openFile(path);
    if (buffer == nullptr)
        return nullptr;
    std::uint8_t* fontBitmap = reinterpret_cast<std::uint8_t*>(buffer);
    return fontBitmap;
}

std::uint16_t* TextController::loadFontPalette(const std::string& path)
{
    void* buffer = openFile(path);
    if (buffer == nullptr)
        return nullptr;
    std::uint16_t* fontPalette = reinterpret_cast<std::uint16_t*>(buffer);
    //dmaCopy(fontPalette, BG_PALETTE, 256 * sizeof(uint16_t));
    return fontPalette;
}

bool TextController::loadFontMetadata(const std::string& path, Font* font)
{
    u32 size;
    void* buffer = openFile(path, size);
    if (buffer == nullptr)
        return false;

    std::string content(reinterpret_cast<char*>(buffer), size);
    free(buffer);

    std::istringstream iss(content);
    std::string line;
    while (std::getline(iss, line))
    {
        if (line.rfind("common ", 0) == 0)
        {
            font->lineHeight = extractIntValue(line, "lineHeight=");
            font->bitmapWidth = extractIntValue(line, "scaleW=");
            font->bitmapHeight = extractIntValue(line, "scaleH=");
            continue;
        }

        //if there's no character in this line, move to the next
        if (line.rfind("char ", 0) != 0)
            continue;

        const int charID = extractIntValue(line, "id=");
        if (charID < 0 || charID >= 256)
            continue;

        Glyph glyph{};
        glyph.xPos = extractIntValue(line, "x=");
        glyph.yPos = extractIntValue(line, "y=");
        glyph.width = extractIntValue(line, "width=");
        glyph.height = extractIntValue(line, "height=");
        glyph.xOffset = extractIntValue(line, "xoffset=");
        glyph.yOffset = extractIntValue(line, "yoffset=");
        font->glyphs[charID] = glyph;
    }

    return true;
}

// Actual Draw Functions =====================================================

void TextController::drawText(
    const std::string& text, Font* font, uint16_t* videoBuffer, int startX, int startY, int color)
{
    int cursorX = startX;
    int cursorY = startY;

    for (char c : text)
    {
        //Handle Newline
        if (c == '\n')
        {
            cursorX = startX;
            cursorY += font->lineHeight + LINE_SPACING;
            continue;
        }

        //Handle automatic word wrapping to prevent drawing outside screen bounds
        if (c == ' ')
        {
            std::string nextWord = "";
            int i = 0; //TODO: set i properly
            while (i < (int)text.size() && text[i] != ' ' && text[i] != '\n')
            {
                nextWord += text[i];
                i++;
            }
            if (checkWordWrap(nextWord, font, cursorX))
            {
                cursorX = startX;
                cursorY += font->lineHeight + 2;
                continue;
            }
            else
            {
                cursorX += SPACE_WIDTH;
                continue;
            }
            if (font->glyphs[static_cast<unsigned char>(c)].width == 0)
            {
                cursorX += SPACE_WIDTH;
                continue;
            }
        }

        Glyph g = font->glyphs[static_cast<unsigned char>(c)];
        drawGlyph(g, font, videoBuffer, cursorX, cursorY, color);
        cursorX += g.width + LETTER_SPACING;
    }
}

void TextController::drawGlyph(
    const Glyph& glyph, Font* font, uint16_t* videoBuffer, int cursorX, int cursorY, int color)
{
    for (int y = 0; y < glyph.height; y++)
    {
        for (int x = 0; x < glyph.width; x++)
        {
            int bitmapX = glyph.xPos + x;
            int bitmapY = glyph.yPos + y;
            int bitmapIndex = bitmapY * font->bitmapWidth + bitmapX;

            sassert(bitmapIndex < font->bitmapWidth * font->bitmapHeight, "Bitmap index out of bounds");

            int pixelValue = font->bitmap[bitmapIndex];
            if (pixelValue > 0)
            {
                int screenX = cursorX + x;
                int screenY = cursorY + glyph.yOffset + y;
                if (screenX >= 0 && screenX < 256 && screenY >= 0 && screenY < 192)
                    drawPixel(videoBuffer, screenX, screenY, color);
            }
        }
    }
}

void TextController::clearScreen(uint16_t* videoBuffer)
{
    dmaFillHalfWords(0, videoBuffer, 256 * 256 * sizeof(uint8_t));
}

// Helper Functions ======================================================

void TextController::drawPixel(uint16_t* videoBuffer, int x, int y, int paletteValue)
{
    int wordIndex = (y * 256 + x) / 2;
    u16 currentWord = videoBuffer[wordIndex];
    if (x % 2 == 0) //Clear the lower 8 bits, then inject our 8-bit color index
        videoBuffer[wordIndex] = (currentWord & 0xFF00) | (paletteValue & 0xFF);
    else //Clear the upper 8 bits, then inject our 8-bit color index shifted up
        videoBuffer[wordIndex] = (currentWord & 0x00FF) | ((paletteValue & 0xFF) << 8);
}

int TextController::extractIntValue(const std::string& line, const std::string& key)
{
    std::size_t keyPos = line.find(key);
    if (keyPos == std::string::npos)
        return 0;

    const std::size_t dataStart = keyPos + key.size();
    std::size_t dataEnd = dataStart;
    while (dataEnd < line.size() && line[dataEnd] != ' ')
        dataEnd++;

    return std::stoi(line.substr(dataStart, dataEnd - dataStart));
}

bool TextController::checkWordWrap(const std::string& text, Font* font, int startX)
{
    int cursorX = startX;
    for (char c : text)
    {
        Glyph g = font->glyphs[static_cast<unsigned char>(c)];
        cursorX += g.width + LETTER_SPACING;
    }
    if (cursorX > 256)
        return true; // Word exceeds screen width
    return false;
}

void TextController::haltOnError(const std::string& errorMessage)
{
    consoleDemoInit();
    iprintf("\n\n");
    iprintf("Error; %s", errorMessage.c_str());
    while (1)
        swiWaitForVBlank();
}

// Test Functions ======================================================

void TextController::testBitmap(Font* font, uint16_t* videoBuffer)
{
    for (int y = 0; y < 256; y++)
    {
        for (int x = 0; x < 256; x++)
        {
            videoBuffer[(y * 256 + x) / 2] = font->bitmap[y * font->bitmapWidth + x] | BIT(15);
        }
    }
}

void TextController::testPalette(Font* font, uint16_t* videoBuffer)
{
    for (int i = 0; i < (256 * 256); i++)
    {
        videoBuffer[i] = customPalette[i % 256] | BIT(15);
    }
}
