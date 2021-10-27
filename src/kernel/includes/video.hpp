#include <stdint.h>
#include <stddef.h>

#ifndef VIDEO_HPP
#define VIDEO_HPP

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_START_ADDRESS 0xb8000

namespace VIDEO {

    namespace VGA {

        class TextMode {
            private:
                // Pointer to video memory address (0xb8000)
                volatile uint16_t * volatile address;
                uint8_t line;
                uint8_t column;

            public:
                volatile uint64_t vLock __attribute__((aligned(8)));

                void clear();

                // Constructor - sets up line, column, address and VGA buffer
                TextMode();

                void putChar(const char character, const uint8_t color);

                size_t putString(const char* string, const uint8_t color);

                size_t putString(const char* string, const uint8_t color,
                                    const size_t length);

                uint8_t getLine() const;

                uint8_t getColumn() const;

                uint8_t setLine(const uint8_t line);

                uint8_t setColumn(const uint8_t column);

                uint16_t getCurrChar();
        };
    };
}

extern VIDEO::VGA::TextMode vgaHandler;

#endif /* VIDEO_HPP */
