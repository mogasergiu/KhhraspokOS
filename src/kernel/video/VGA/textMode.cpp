#include <video.hpp>
#include <kstring.hpp>
#include <io.hpp>

using namespace VIDEO;

VIDEO::VGA::TextMode vgaHandler;

void VGA::TextMode::clear() {
     // Clear screen (fill with black zeroes)
    for (uint8_t l = 0; l < VGA_HEIGHT; l++) {
        for (uint8_t c = 0; c < VGA_WIDTH; c++) {
            this->address[l * VGA_WIDTH + c] = 0x0032;
        }
    }
}  

// Constructor - sets up line, column, address and VGA buffer
VGA::TextMode::TextMode() {
    this->column = this->line = 0;

    this->address = (uint16_t*)VGA_START_ADDRESS;

    this->clear();

    // disable cursor
    PMIO::pOutByte(0x3D4, 0x0A);
    PMIO::pOutByte(0x3D5, 0x20);
}

/*
 * Places given character of given color in VGA buffer
 */
void __attribute__((optimize("O3"))) VGA::TextMode::putChar(const char character, const uint8_t color) {
    if (this->line == VGA_HEIGHT) {
        // Scroll Up!
        for (int i = 0; i < VGA_HEIGHT - 1; i++) {
            for (int j = 0; j < VGA_WIDTH; j++) {
                this->address[i * VGA_WIDTH + j] =
                                        this->address[(i + 1) * VGA_WIDTH + j];
            }
        }

        for (int i = 0; i < VGA_WIDTH; i++) {
            this->address[(VGA_HEIGHT - 1) * VGA_WIDTH + i] = 0;
        }

        this->column = 0;
        this->line--;
    }
    // If character is newline, move to next line and reset column
    if (character == '\n') {
        for (int j = this->column; j < VGA_WIDTH; j++) {
            this->address[this->line * VGA_WIDTH + j] = 0x0032;
        }

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
 */
uint8_t VGA::TextMode::getLine() const {
    return this->line;
}

/*
 * Retrieves current column of our position in the VGA buffer
 */
uint8_t VGA::TextMode::getColumn() const {
    return this->column;
}

/*
 * Sets current line in the VGA buffer
 */
uint8_t VGA::TextMode::setLine(const uint8_t line) {
    if (line <= VGA_HEIGHT) {
        this->line = line;

    }

    return this->line;
}

/*
 * Sets current column in the VGA buffer
 */
uint8_t VGA::TextMode::setColumn(const uint8_t column) {
    if (column <= VGA_WIDTH) {
        this->column = column;
    }

    return this->column;
}

uint16_t VGA::TextMode::getCurrChar() {
    return this->address[VGA_WIDTH * this->line + this->column - 1];
}
