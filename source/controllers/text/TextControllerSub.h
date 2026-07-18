/**
 * @file TextControllerSub.h
 * @brief Controller for rendering text from a bitmap on the Nintendo DS.
 * @author ggmini
 */

#include "core/structs.h"
#include "nds.h"
#include <cstdint>
#include <string>

/**
 * @brief A class that handles rendering text from a bitmap on the Nintendo DS.
 *
 * This class provides functionality to load a font from a bitmap and render text to the sub screen using that font.
 */
class TextControllerSub
{
  public:
    TextControllerSub(const std::string& fontFilePath = "cosmetica/size-8/size-8"); //TODO: REMOVE DEFAULT
    ~TextControllerSub();

    /**
     * @brief Draws the specified text at the given coordinates with the specified color.
     *
     * @param text The text to be drawn.
     * @param x The x-coordinate where the text will start.
     * @param y The y-coordinate where the text will start.
     * @param color The color of the text in ARGB16 format. Default is white.
     */
    void drawText(const std::string& text, int x, int y, int color = ARGB16(1, 31, 31, 31));
    void drawGlyph(const Glyph& glyph, int x, int y, int color);
    /**
     * @brief Clears the screen.
     *
     * This function resets all pixels of the background, effectively clearing any previously drawn content.
     * Should be used when removing text from screen, i.e. to draw new content.
     */
    void clearScreen();

    /**
     * @brief Draws the bitmap to the screen for testing purposes.
     *
     * This funnction is intended for testing the bitmap.
     * It draws the entire bitmap to the screen.
     */
    void testBitmap();
    /**
     * @brief Draws the palette to the screen for testing purposes.
     *
     * This function is intended for testing the palette.
     * It draws the entire palette to the screen.
     */
    void testPalette();

  private:
    /**
     * @brief Loads the font bitmap from a file.
     *
     * @param path The path to the font bitmap file.
     * @return true if the font bitmap was loaded successfully, false otherwise.
     * @retval true The font bitmap was loaded successfully.
     * @retval false The font bitmap could not be loaded.
     */
    bool loadFontBitmap(const std::string& path);
    /**
     * @brief Loads the font palette from a file.
     *
     * @param path The path to the font palette file.
     * @return true if the font palette was loaded successfully, false otherwise.
     * @retval true The font palette was loaded successfully.
     * @retval false The font palette could not be loaded.
     */
    bool loadFontPalette(const std::string& path);
    /**
     * @brief Loads the font metadata from a file.
     *
     * @param path The path to the font metadata file.
     * @return true if the font metadata was loaded successfully, false otherwise.
     * @retval true The font metadata was loaded successfully.
     * @retval false The font metadata could not be loaded.
     */
    bool loadFontMetadata(const std::string& path);
    /**
     * @brief Opens a file and reads its contents into memory.
     *
     * @param path The path to the file to be opened.
     * @return A pointer to the memory buffer containing the file's contents, or nullptr if the file could not be opened.
     * @note The caller is responsible for freeing the memory buffer when it is no longer needed.
     */
    void* openFile(const std::string& path);
    /**
     * @brief Opens a file and reads its contents into memory.
     *
     * @param path The path to the file to be opened.
     * @param size A reference to a variable that will hold the size of the file in bytes.
     * @return A pointer to the memory buffer containing the file's contents, or nullptr if the file could not be opened.
     * @note The caller is responsible for freeing the memory buffer when it is no longer needed.
     */
    void* openFile(const std::string& path, u32& size);

    /**
     * @brief Draws a single pixel at the specified coordinates with the specified color.
     *
     * @param x The x-coordinate of the pixel to be drawn.
     * @param y The y-coordinate of the pixel to be drawn.
     * @param paletteIndex The index of the color in the palette to be used for the pixel.
     */
    void drawPixel(int x, int y, int paletteIndex);
    /**
     * @brief Checks if next word in a string will exceed the screen width when drawn.
     *
     * @param text The text to be printed.
     * @param startX The x-coordinate from which to start drawing the word.
     * @return true if the next word will exceed the screen width, false otherwise.
     */
    bool checkWordWrap(const std::string& text, int startX);
    /**
     * @brief Extracts an integer value from a line of text based on a specified key.
     *
     * @param line The line of text from which to extract the integer value.
     * @param key The key that precedes the integer value in the line of text.
     * @return The integer value extracted from the line of text.
     * @note This function assumes that the line takes the form "key=value ".
     */
    int extractIntValue(const std::string& line, const std::string& key);
    /**
     * @brief Halts the program and displays an error message.
     *
     * @param errorMessage The error message to be displayed.
     * @note This function will enter an infinite loop after displaying the error message, effectively halting the program.
     * It is intended for use in critical error situations where continuing execution could lead to undefined behavior or further errors.
     */
    void haltOnError(const std::string& errorMessage);

    uint16_t* videoBuffer;
    std::uint8_t* fontBitmap;
    std::uint16_t* fontPalette;
    int fontBitmapWidth;
    int fontBitmapHeight;
    int fontLineHeight;
    uint16_t bgID;

    int LETTER_SPACING = 1;
    int SPACE_WIDTH = 4;
    Glyph fontGlyphs[256];
};
