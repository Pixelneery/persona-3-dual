/**
 * @file TextController.h
 * @brief Controller for rendering text from a bitmap on the Nintendo DS.
 * @author ggmini
 */

#include "nds.h"
#include <cstdint>
#include <string>

/**
 * @brief Stores data for a single glyph (character) in a font.
 */
struct Glyph
{
    int xPos;
    int yPos;
    int width;
    int height;
    int xOffset;
    int yOffset;
};

/**
 * @brief Stores data for a font.
 */
struct Font
{
    std::uint8_t* bitmap = nullptr;
    int bitmapWidth = 256;
    int bitmapHeight = 256;
    int lineHeight = 32;
    Glyph glyphs[256];
};

/**
 * @brief Human Readable enum for text colors.
 */
enum TextColor
{
    Transparent = 0,
    Black = 1,
    White = 2,
    DualGreen = 3,
    DualGreen2 = 4,
    DarkGreen = 5,
    DarkerGreen = 6,
    DarkestGreen = 7,
    LightBlue = 8,
    RichBlue = 9,
    DarkBlue = 10,
    NavyBlue = 11,
    DarkestBlue = 12,
    LightOrange = 13,
    LightPurple = 14,
    Red = 15,
    Green = 16,
    Blue = 17,
    Yellow = 18,
    Magenta = 19,
    Cyan = 20,
    Gray = 21
};

/**
 * @brief A struct that represents a block of text being rendered on the screen.
 */
struct Text
{
    int cursorX;
    int cursorY;
    int startX;
    int startY;
    std::string content;
    int color;
    Font* font;
    uint16_t* videoBuffer;
    int cursorPos;
    int counter;
};

/**
 * @brief A class that handles rendering text from a bitmap on the Nintendo DS.
 *
 * This class provides functionality to load a font from a bitmap and render text to the screen.
 */

class TextController
{
  public:
    /**
   * @brief Get the singleton instance of the TextController.
   * @return Pointer to the singleton instance of TextController.
   */
    static TextController* getInstance()
    {
        static TextController instance;
        return &instance;
    }

    /**
     * @brief Frame update function to be called every frame.
     */
    void update();

    /**
     * @brief Load a font from a file.
     * @param fontFilePath The path to the font file without file extension.
     * @return Pointer to the loaded font, or nullptr if loading failed.
     */
    Font* loadFont(const std::string& fontFilePath = "cosmetica/size-32/size-32");

    /**
     * @brief Loads the predefined default palette.
     */
    void loadDefaultPalette();

    /**
     * @brief Load a palette from a file.
     * @param paletteFilePath The path to the palette file.
     * @param sub Whether to load the palette for the sub screen.
     * @return true if the palette was loaded successfully, false otherwise.
     * @note This palette will apply for all text that is being drawn.
     */
    bool loadPalette(const std::string& paletteFilePath, bool sub = false);

    /**
     * @brief Unload the current palettes.
     * @note This function unloads the palettes from both the main and sub screens.
     */
    void unloadPalette();

    /**
     * @brief Draw text to the screen.
     * @param text The text to draw.
     * @param font Pointer to the font to use for rendering.
     * @param videoBuffer Pointer to the video buffer to draw to.
     * @param x The x-coordinate to start drawing the text.
     * @param y The y-coordinate to start drawing the text.
     * @param color The color to use for the text (default is white).
     */
    void drawText(
        const std::string& text, Font* font, uint16_t* videoBuffer, int x, int y, int color = ARGB16(1, 31, 31, 31));
    /**
     * @brief Create a Text object and renders each character with a delay to simulate typing effect.
     * @param text The text to render.
     * @param font Pointer to the font to use for rendering.
     * @param videoBuffer Pointer to the video buffer to draw to.
     * @param x The x-coordinate to start drawing the text.
     * @param y The y-coordinate to start drawing the text.
     * @param color The color to use for the text (default is white).
     */
    void appearText(
        const std::string& text, Font* font, uint16_t* videoBuffer, int x, int y, int color = ARGB16(1, 31, 31, 31));
    /**
     * @brief If a text is currently being rendered with appearText, this function will immediately render the rest of the text without delay.
     */
    void appearTextSkip();
    /**
     * @brief Draw a single glyph to the screen.
     * @param glyph The glyph to draw.
     * @param font Pointer to the font that contains the glyph.
     * @param videoBuffer Pointer to the video buffer to draw to.
     * @param x The x-coordinate to start drawing the glyph.
     * @param y The y-coordinate to start drawing the glyph.
     * @param color The color to use for the glyph.
     */
    void drawGlyph(const Glyph& glyph, Font* font, uint16_t* videoBuffer, int x, int y, int color);
    /**
     * @brief Clear the text layer by filling the video buffer with black.
     * @param videoBuffer Pointer to the video buffer to clear.
     */
    void clearScreen(uint16_t* videoBuffer);

    /**
     * @brief Test function to draw the font bitmap to the screen.
     * @param font Pointer to the font to test.
     * @param videoBuffer Pointer to the video buffer to draw to.
     * @note This function is intended for testing purposes.
     */
    void testBitmap(Font* font, uint16_t* videoBuffer);
    /**
     * @brief Test function to draw the currently loaded palette to the screen.
     * @param videoBuffer Pointer to the video buffer to draw to.
     * @note This function is intended for testing purposes.
     */
    void testPalette(uint16_t* videoBuffer);

  private:
    Text* text;
    int APPEAR_DELAY = 5;
    int LETTER_SPACING = 1;
    int SPACE_WIDTH = 4;

    TextController();
    TextController(const TextController&) = delete;
    TextController& operator=(const TextController&) = delete;

    // Loader Functions
    /**
     * @brief Load the font bitmap from a file.
     * @param path The path to the font bitmap file.
     * @return Pointer to the loaded font bitmap, or nullptr if loading failed.
     */
    std::uint8_t* loadFontBitmap(const std::string& path);
    /**
     * @brief Load the font metadata from a file.
     * @param path The path to the font metadata file.
     * @param font Pointer to the font to populate with metadata.
     * @return true if loading was successful, false otherwise.
     */
    bool loadFontMetadata(const std::string& path, Font* font);

    // Helper Functions
    /**
     * @brief Open a file and return a pointer to its contents.
     * @param path The path to the file to open.
     * @return Pointer to the contents of the file, or nullptr if opening failed.
     */
    void* openFile(const std::string& path);
    /**
     * @brief Open a file and return a pointer to its contents, along with the size of the file.
     * @param path The path to the file to open.
     * @param size Reference to a variable to store the size of the file.
     * @return Pointer to the contents of the file, or nullptr if opening failed.
     * @note This function is useful when you need to know the size of the file being opened.
     */
    void* openFile(const std::string& path, u32& size);
    /**
     * @brief Draw the next character from the text struct to the screen.
     * @note Used to draw text which appears character by character.
     */
    void drawNextFromText(Text* text);
    /**
     * @brief Create a Text object and initialize its properties.
     * @param text The text content for the Text object.
     * @param font Pointer to the font to use for rendering the text.
     * @param videoBuffer Pointer to the video buffer to draw to.
     * @param startX The x-coordinate to start drawing the text.
     * @param startY The y-coordinate to start drawing the text.
     * @param color The color to use for the text.
     * @return Pointer to the newly created Text object.
     */
    Text* createText(const std::string& text, Font* font, uint16_t* videoBuffer, int startX, int startY, int color);
    /**
     * @brief Draw a single pixel to the video buffer.
     * @param videoBuffer Pointer to the video buffer to draw to.
     * @param x The x-coordinate of the pixel to draw.
     * @param y The y-coordinate of the pixel to draw.
     * @param paletteValue The color index in the palette to use for the pixel.
     */
    void drawPixel(uint16_t* videoBuffer, int x, int y, int paletteValue);
    /**
     * @brief Get the next word from a given text string.
     * @param text The text string to extract the next word from.
     * @return The next word in the text string, or an empty string if there are no more words.
     */
    std::string getNextWord(const std::string& text);
    /**
     * @brief Check if a given text string will exceed the screen width when rendered with the specified font.
     * @param text The text string to check.
     * @param font Pointer to the font to use for rendering.
     * @param startX The starting x-coordinate for rendering the text.
     * @return true if the text will exceed the screen width, false otherwise.
     */
    bool checkWordWrap(const std::string& text, Font* font, int startX);
    /**
     * @brief Extract an integer value from a line of text based on a specified key.
     * @param line The line of text to extract the value from.
     * @param key The key that preceedes the integer value in the line.
     * @return The extracted integer value, or 0 if the key is not found.
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
};
