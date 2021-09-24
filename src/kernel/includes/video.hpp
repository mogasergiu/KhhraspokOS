#include <stdint.h>
#include <stddef.h>

#ifndef VIDEO_HPP
#define VIDEO_HPP

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_START_ADDRESS 0xb8000

/*********************************************************
 * This namespace contains every Video related component *
 *********************************************************/
namespace VIDEO {

    /*******************************************************
     * This namespace contains every VGA related component *
     *******************************************************/
    namespace VGA {

        /****************************************************************
         * This class contains the Text Mode functionality, part of VGA *
         ****************************************************************/
        class TextMode {
            private:
                // Pointer to video memory address (0xb8000)
                uint16_t *address;

                // Represents the line we are currently at inside VGA buffer
                uint8_t line;

                // Represents the column we are currently at inside VGA buffer
                uint8_t column;

            public:
                volatile uint64_t vLock __attribute__((aligned(8)));

                // Constructor - sets up line, column, address and VGA buffer
                TextMode();

                /*
                 * Places given character of given color in VGA buffer
                 * @character: the character the be placed in the buffer
                 * @color: the desired color of the character
                 */
                void putChar(const char character, const uint8_t color);

                /*
                 * Places given string of given color in VGA buffer
                 * @string: string to be placed in VGA buffer
                 * @color: desired color of the string
                 * @return: number of bytes successfully placed in VGA buffer
                 */
                size_t putString(const char* string, const uint8_t color);

                /*
                 * Places first number of given bytes of given string of given
                 * color in VGA buffer
                 * @string: string to be placed in VGA buffer
                 * @color: desired color of the string
                 * @length: number of first bytes of given string to be placed
                 * @return: number of bytes successfully placed in VGA buffer
                 */
                size_t putString(const char* string, const uint8_t color,
                                    const size_t length);

                /*
                 * Retrieves current line of our position in the VGA buffer
                 * @return: current line of our position in the VGA buffer
                 */
                uint8_t getLine() const;

                /*
                 * Retrieves current column of our position in the VGA buffer
                 * @return: current column of our position in the VGA buffer
                 */
                uint8_t getColumn() const;

                /*
                 * Sets current line in the VGA buffer
                 * @line: given line to set
                 * @return: the new line or the current line if given line is
                 *           out of bounds
                 */
                uint8_t setLine(const uint8_t line);

                /*
                 * Sets current column in the VGA buffer
                 * @column: given column to set
                 * @return: the new column or the current column if given
                 *          column is out of bounds
                 */
                uint8_t setColumn(const uint8_t column);
        };
    };
}

extern VIDEO::VGA::TextMode vgaHandler;

#endif /* VIDEO_HPP */
