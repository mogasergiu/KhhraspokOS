#include <interrupts.hpp>
#include <drivers.hpp>
#include <kstdio.hpp>
#include <video.hpp>

using namespace DRIVERS::PS2::KEYBOARD;
using namespace INTERRUPTS;

char DRIVERS::PS2::KEYBOARD::buffer[KEYBOARD_BUFFER_MAX_SIZE];

// Code Table
static uint8_t keyboardCode[] = {
    0x00, 0x1B, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
    0x08, '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']',
    0x0d, 0x00, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`', 
    0x00, '\\', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', 0x00, '*',
    0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.'
};

static uint8_t keyboardCodeSize = sizeof(keyboardCode) / sizeof(*keyboardCode);
static uint32_t buffIdx, readIdx;
static bool capitalON, lineON;

void __attribute__((constructor)) initKeyboard() {
    PMIO::pOutByte(PS2_PORT, PS2_ENABLE_KEYBOARD);
}

static void putInBuffer(char c) {
    if (buffIdx == KEYBOARD_BUFFER_MAX_SIZE) {
        buffIdx = 0;
    }

    buffer[buffIdx++] = c;
}

// Used by gets-like functions
size_t DRIVERS::PS2::KEYBOARD::readKeyboard(char *buf, size_t size) {
    if (!lineON) {
        return 0;
    }

    size = size < KEYBOARD_BUFFER_MAX_SIZE ? size : KEYBOARD_BUFFER_MAX_SIZE;
    for (size_t i = 0; i < size; i++, readIdx++) {
        if (readIdx == KEYBOARD_BUFFER_MAX_SIZE) {
            readIdx = 0;
        }

        if (buffer[readIdx] == '\n') {
            buf[i] = 0;

            lineON = false;

            readIdx++;

            return i + 1;

        } else {
            buf[i] = buffer[readIdx];
        }
    }

    lineON = false;

    return 0;
}

static inline void deleteChar() {
    if (vgaHandler.getCurrChar() != 0xa3e) {
        vgaHandler.setColumn(vgaHandler.getColumn() - 1);
        vgaHandler.putChar(0x32, 0);
        vgaHandler.setColumn(vgaHandler.getColumn() - 1);
        buffIdx--;
    }
}

// Gets executed when keyboard button gets pressed
extern "C" void IntCallbacks::keyboardIRQ() {
    uint8_t scanCode = PMIO::pInByte(KEYBOARD_INPUT_PORT);
    PMIO::pInByte(KEYBOARD_INPUT_PORT);

    if (scanCode == KEYBOARD_SPACE_PRESSED) {
        putInBuffer(' ');
        kputc(' ');

    } else if (scanCode == KEYBOARD_BACKSPACE_PRESSED) {
        deleteChar();

    } else if (scanCode == KEYBOARD_ENTER_PRESSED) {
        if (!lineON) {
            putInBuffer('\n');
            lineON = true;
            kputc('\n');
        }

    } else if (scanCode == KEYBOARD_CAPSLOCK_PRESSED ||
        scanCode == KEYBOARD_RSHIFT_PRESSED ||
        scanCode == KEYBOARD_RSHIFT_RELEASED ||
        scanCode == KEYBOARD_LSHIFT_PRESSED ||
        scanCode == KEYBOARD_LSHIFT_RELEASED) {
            capitalON = !capitalON;

    } else {
        uint8_t c = scanCode > keyboardCodeSize ? 0 :
                    capitalON ? keyboardCode[scanCode] :
                    keyboardCode[scanCode] + 32;

        if (c) {
            putInBuffer(c);
            kputc(c);
        }
    }

    apicHandler.sendLAPICEOI();
}

