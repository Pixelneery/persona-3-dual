/**
 * @file TextController.h
 * @brief Controller for rendering text from a bitmap on the Nintendo DS.
 * @author ggmini
 */

#include "core/structs.h"
#include "nds.h"
#include <cstdint>
#include <string>

struct Text
{
    std::string content;
    int x;
    int y;
    int color;
};

/**
 * @brief A class that handles rendering text from a bitmap on the Nintendo DS.
 *
 * This class provides functionality to load a font from a bitmap and render text to the screen using that font.
 */
class TextController
{
  public:
    TextController(const std::string& fontFilePath = "skip-std-b/size-8/size-8"); //TODO: REMOVE DEFAULT
    ~TextController();

    /**
     * @brief Draws the specified text at the given coordinates with the specified color.
     *
     * @param text The text to be drawn.
     * @param x The x-coordinate where the text will start.
     * @param y The y-coordinate where the text will start.
     * @param color The color of the text in ARGB16 format. Default is white.
     */
    void drawText(const std::string& text, int x, int y, int color = ARGB16(1, 31, 31, 31));
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
    uint16_t* videoBuffer;

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

    std::uint8_t* fontBitmap;
    std::uint16_t* fontPalette;
    int fontBitmapWidth;
    int fontBitmapHeight;
    int fontLineHeight;
    uint16_t bgID; // Store the background ID for cleanup

    Glyph fontGlyphs[256];

    void drawPixel(int x, int y, int color);
    bool checkWordWrap(const std::string& text, int startX);
    int extractPixelValue(const int pixelIndex);
    int extractIntValue(const std::string& line, const std::string& key);
    void haltOnError(const std::string& errorMessage);
};
