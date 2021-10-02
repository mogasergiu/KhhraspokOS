#include <interrupts.hpp>
#include <kstdio.hpp>

using namespace INTERRUPTS;

uint32_t INTERRUPTS::ticks = 0;
INTERRUPTS::PIT pitHandler;

extern "C" void IntCallbacks::pitIRQ() {
    INTERRUPTS::ticks++;

    // PMIO::pOutByte(PIC1_REG_COMMAND, PIC_EOI);
    apicHandler.sendLAPICEOI();
}

void PIT::sleep(uint64_t ms30) const {
    uint64_t start = INTERRUPTS::ticks;

    for (; INTERRUPTS::ticks - start < ms30;);
}

// Constructor - Initializes the PIT
void PIT::initPIT() {
    this->sendOCW(PIT_OCW_BIN | PIT_OCW_MODE_SQUAREWAVEGENERATOR | PIT_OCW_RL_LMSB);

    // send LSB
    this->sendChannel((INPUT_FREQ / DIVISOR) & 0xff, PIT_REG_CHANNEL_0);

    // send MSB
    this->sendChannel(((INPUT_FREQ / DIVISOR) & 0xff00) >> 8, PIT_REG_CHANNEL_0);

    ticks = 0;
}

/*
* Getter method to retrieve the number of system ticks
*/
uint32_t PIT::getTicks() const {
    return ticks;
}

/*
* Write Operation Command Word to Command Register
*/
void PIT::sendOCW(uint8_t ocw) const {
    PMIO::pOutByte(PIT_REG_CW, ocw);
};

/*
* Get current count value from Channel Register
*/
uint8_t PIT::readChannel(uint8_t channel) const {
    switch (channel) {
        case PIT_OCW_SC_0:
            return PMIO::pInByte(PIT_REG_CHANNEL_0);
        case PIT_OCW_SC_1:
            return PMIO::pInByte(PIT_REG_CHANNEL_1);
        default:
            return PMIO::pInByte(PIT_REG_CHANNEL_2);
    }
}

/*
* Set current count value of Channel Register
*/
void PIT::sendChannel(uint8_t channel, uint8_t value) const {
    switch (channel) {
        case PIT_OCW_SC_0:
            return PMIO::pOutByte(PIT_REG_CHANNEL_0, value);
        case PIT_OCW_SC_1:
            return PMIO::pOutByte(PIT_REG_CHANNEL_1, value);
        default:
            return PMIO::pOutByte(PIT_REG_CHANNEL_2, value);
    }
}

