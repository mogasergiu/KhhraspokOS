#include <interrupts.hpp>
#include <drivers.hpp>
#include <kstdio.hpp>

using namespace DRIVERS::PS2::KEYBOARD;
using namespace INTERRUPTS;

char buffer[KEYBOARD_BUFFER_MAX_SIZE];

static uint8_t keyboardCode[] = {
    0x00, 0x1B, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
    0x08, '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']',
    0x0d, 0x00, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`', 
    0x00, '\\', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', 0x00, '*',
    0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.'
};

static uint8_t keyboardCodeSize = sizeof(keyboardCode) / sizeof(*keyboardCode);

static bool capitalON;

void __attribute__((constructor)) initKeyboard() {
    PMIO::pOutByte(PS2_PORT, PS2_ENABLE_KEYBOARD);
}

extern "C" void IntCallbacks::keyboardIRQ() {
    uint8_t scanCode = PMIO::pInByte(KEYBOARD_INPUT_PORT);
    PMIO::pInByte(KEYBOARD_INPUT_PORT);

    if (scanCode == KEYBOARD_SPACE_PRESSED) {
        kputc(' ');

    } else if (scanCode == KEYBOARD_ENTER_PRESSED) {
        kputc('\n');

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
            kputc(c);
        }
    }

    apicHandler.sendLAPICEOI();
}

