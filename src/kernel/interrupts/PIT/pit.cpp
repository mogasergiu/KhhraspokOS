#include <interrupts.hpp>

using namespace INTERRUPTS;

uint32_t INTERRUPTS::ticks = 0;

extern "C" void IntCallbacks::pitIRQ() {
    INTERRUPTS::ticks++;

    PIO::outByte(PIC1_REG_COMMAND, PIC_EOI);
}

// Constructor - Initializes the PIT
PIT::PIT() {
    this->sendOCW(PIT_OCW_BIN | PIT_OCW_MODE_SQUAREWAVEGENERATOR | PIT_OCW_RL_LMSB);

    // send LSB
    this->sendChannel((INPUT_FREQ / DIVISOR) & 0xff, PIT_REG_CHANNEL_0);

    // send MSB
    this->sendChannel(((INPUT_FREQ / DIVISOR) & 0xff00) >> 8, PIT_REG_CHANNEL_0);

    ticks = 0;
}

/*
* Getter method to retrieve the number of system ticks
* @return: number of system ticks
*/
uint32_t PIT::getTicks() const {
    return ticks;
}

/*
* Write Operation Command Word to Command Register
* @ocw: the OCW to send
*/
void PIT::sendOCW(uint8_t ocw) const {
    PIO::outByte(PIT_REG_CW, ocw);
};

/*
* Get current count value from Channel Register
* @channel: the Channel Register we get the value from
* @return: value found in the Channel Register
*/
uint8_t PIT::readChannel(uint8_t channel) const {
    switch (channel) {
        case PIT_OCW_SC_0:
            return PIO::inByte(PIT_REG_CHANNEL_0);
        case PIT_OCW_SC_1:
            return PIO::inByte(PIT_REG_CHANNEL_1);
        default:
            return PIO::inByte(PIT_REG_CHANNEL_2);
    }
}

/*
* Set current count value of Channel Register
* @channel: the Channel Register whose value we set
* @value: the value we will set
* @return: value found in the Channel Register
*/
void PIT::sendChannel(uint8_t channel, uint8_t value) const {
    switch (channel) {
        case PIT_OCW_SC_0:
            return PIO::outByte(PIT_REG_CHANNEL_0, value);
        case PIT_OCW_SC_1:
            return PIO::outByte(PIT_REG_CHANNEL_1, value);
        default:
            return PIO::outByte(PIT_REG_CHANNEL_2, value);
    }
}
