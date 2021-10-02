#include <interrupts.hpp>

using namespace INTERRUPTS;

INTERRUPTS::PIC picHandler;

// Constructor - sets up the Programmable Interrupt Controller
void PIC::initPIC() {
    uint8_t icw = 0;

    // Send ICW1
    this->writeCMDR(icw | PIC_ICW1_INIT | PIC_ICW1_IC4, PIC1);
    this->writeCMDR(icw | PIC_ICW1_INIT | PIC_ICW1_IC4, PIC2);

    // Send ICW2
    this->writeIMR(PIC1_IRQ0, PIC1);
    this->writeIMR(PIC2_IRQ8, PIC2);

    // Send ICW3
    this->writeIMR(PIC1_ICW3_PIC2_LINK, PIC1);
    this->writeIMR(PIC2_ICW3_PIC1_LINK, PIC2);
    
    // Send ICW4
    this->writeIMR(PIC_ICW4_UPM, PIC1);
    this->writeIMR(PIC_ICW4_UPM, PIC2);

    // Mask all interrupts
    this->writeIMR(0xff, PIC1);
    this->writeIMR(0xff, PIC2);

};

/*
 * Function to write to the PIC'S Command Register
 */
void PIC::writeCMDR(uint8_t cmd, uint8_t picNum) {
    if (picNum == 1) {
        PMIO::pOutByte(PIC2_REG_COMMAND, cmd);

    } else if (picNum == 0) {
        PMIO::pOutByte(PIC1_REG_COMMAND, cmd);
    }
}

/*
 * Function to write to the PIC'S Interrupt Mask Register
 */
void PIC::writeIMR(uint8_t data, uint8_t picNum) {
    if (picNum == 1) {
        PMIO::pOutByte(PIC2_REG_IMR, data);

    } else if (picNum == 0) {
        PMIO::pOutByte(PIC1_REG_IMR, data);
    }
}

/*
 * Function to read from the PIC'S Data Register
 */
uint8_t PIC::readDR(uint8_t picNum) {
    uint8_t portNum;

    if (picNum == 1) {
        portNum = PIC2_REG_DATA;

    } else if (picNum == 0) {
        portNum = PIC1_REG_DATA;
    }

    return PMIO::pInByte(portNum);
}

/*
 * Function to read from the PIC'S portNum Register
 */
uint8_t PIC::readSR(uint8_t picNum) {
    uint8_t portNum;

    if (picNum == 1) {
        portNum = PIC2_REG_STATUS;

    } else if (picNum == 0) {
        portNum = PIC1_REG_STATUS;
    }

    return PMIO::pInByte(portNum);
}


