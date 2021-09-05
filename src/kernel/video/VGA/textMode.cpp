#include <video.hpp>
#include <kstring.hpp>

using namespace VIDEO;

VIDEO::VGA::TextMode vgaHandler;

// Constructor - sets up line, column, address and VGA buffer
VGA::TextMode::TextMode() {
    this->column = this->line = 0;

    this->address = (uint16_t*)VGA_START_ADDRESS;

    // Clear screen (fill with black zeroes)
    for (uint8_t l = 0; l < VGA_HEIGHT; l++) {
        for (uint8_t c = 0; c < VGA_WIDTH; c++) {
            this->address[l * VGA_WIDTH + c] = 0x0032;
        }
    }
}

/*
 * Places given character of given color in VGA buffer
 * @character: the character the be placed in the buffer
 * @color: the desired color of the character
 */
void VGA::TextMode::putChar(const char character, const uint8_t color) {
    if (this->line == VGA_HEIGHT) {
        // Clear screen (fill with black zeroes)
        for (uint8_t l = 0; l < VGA_HEIGHT; l++) {
            for (uint8_t c = 0; c < VGA_WIDTH; c++) {
                this->address[l * VGA_WIDTH + c] = 0x0032;
            }
        }

        this->line = 0;
        this->column = 0;
    }
    // If character is newline, move to next line and reset column
    if (character == '\n') {
        this->line++;
        this->column = 0;

    } else {
        // Pace character of given color in VGA buffer at our current position
        this->address[VGA_WIDTH * this->line + this->column] =
                                                    (color << 8) + character;

        // Move cursor forward
        this->column++;

        // If end of the line reached, we move to next line and reset column
        if (this->column >= VGA_WIDTH) {
            this->column = 0;
            this->line++;
        }
    }
}

/*
 * Places given string of given color in VGA buffer
 * @string: string to be placed in VGA buffer
 * @color: desired color of the string
 * @return: number of bytes successfully placed in VGA buffer
 */
size_t VGA::TextMode::putString(const char* string, const uint8_t color) {
    size_t length = strlen(string), l;

    for (l = 0; l < length; l++) {
        putChar(string[l], color);
    }

    return l;
}

/*
 * Places first number of given bytes of given string of given
 * color in VGA buffer
 * @string: string to be placed in VGA buffer
 * @color: desired color of the string
 * @length: number of first bytes of given string to be placed
 * @return: number of bytes successfully placed in VGA buffer
 */
size_t VGA::TextMode::putString(const char* string, const uint8_t color,
                                        const size_t length) {
    size_t l;

    for (l = 0; l < length; l++) {
        putChar(string[l], color);
    }

    return l;
}

/*
 * Retrieves current line of our position in the VGA buffer
 * @return: current line of our position in the VGA buffer
 */
uint8_t VGA::TextMode::getLine() const {
    return this->line;
}

/*
 * Retrieves current column of our position in the VGA buffer
 * @return: current column of our position in the VGA buffer
 */
uint8_t VGA::TextMode::getColumn() const {
    return this->column;
}

/*
 * Sets current line in the VGA buffer
 * @line: given line to set
 * @return: the new line or the current line if given line is
 *           out of bounds
 */
uint8_t VGA::TextMode::setLine(const uint8_t line) {
    if (line <= VGA_HEIGHT) {
        this->line = line;

    }

    return this->line;
}

/*
 * Sets current column in the VGA buffer
 * @column: given column to set
 * @return: the new column or the current column if given
 *          column is out of bounds
 */
uint8_t VGA::TextMode::setColumn(const uint8_t column) {
    if (column <= VGA_WIDTH) {
        this->column = column;
    }

    return this->column;
}

