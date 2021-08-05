#include <interrupts.hpp>

using namespace INTERRUPTS;

// Constructor - sets up IDTR, zeroes out IDT and loads IDT (lidt)
Interrupts::Interrupts() {
    for (uint32_t i = 0; i < sizeof(this->IDTDescriptors); i++) {
        ((char*)this->IDTDescriptors)[i] = 0;
    }

    this->IDTRDescriptor.limit = sizeof(this->IDTDescriptors) - 1;
    this->IDTRDescriptor.base = (uint32_t) this->IDTDescriptors;

    __asm__ __volatile__ (
        "movl %0, %%esi;"
        "lidt 0(%%esi);"
        :
        : "r" ( &this->IDTRDescriptor )
    );
}

/*
 * Helper function to add an entry in the IDT
 * @number: desired interrupt number of the entry
 * @address: address of the function to be placed in the entry
 */
void Interrupts::setIDTEntry(uint32_t number, void (*address)()) {
    IDTD *descriptor = this->IDTDescriptors + number;

    // Lower part of the interrupt function's offset address
    descriptor->offset1 = (uint32_t)address & 0x0000ffff;

    // Code Segment from GDT
    descriptor->selector = 0x8;

    // Must always be zero
    descriptor->zero = 0x0;

    // P = 1b, DPL = 00b, S = 0b, type = 1110b
    descriptor->typeAttr = 0x8E;

    // Higher part of the interrupt function's offset address
    descriptor->offset2 = (uint32_t)address >> 0x10;
}

// Constructor - sets up the Programmable Interrupt Controller
PIC::PIC() {
    uint8_t icw = 0;

    // Send ICW1
    this->writeCMDR(icw | PIC_ICW1_INIT | PIC_ICW1_IC4, PIC1);
    this->writeCMDR(icw | PIC_ICW1_INIT | PIC_ICW1_IC4, PIC2);

    // Send ICW2
    this->writeIMR(PIC1_ICW2_IRQ0, PIC1);
    this->writeIMR(PIC2_ICW2_IRQ8, PIC2);

    // Send ICW3
    this->writeIMR(PIC1_ICW3_PIC2_LINK, PIC1);
    this->writeIMR(PIC2_ICW3_PIC1_LINK, PIC2);
    
    // Send ICW4
    this->writeIMR(PIC_ICW4_UPM, PIC1);
    this->writeIMR(PIC_ICW4_UPM, PIC2);
};

/*
 * Function to write to the PIC'S Command Register
 * @cmd: the command byte to send
 * @picNum: which PIC to send the command to (0 for Master,
 * 1 for Slave)
 */
void PIC::writeCMDR(uint8_t cmd, uint8_t picNum) {
    if (picNum == 1) {
        PIO::outByte(PIC2_REG_COMMAND, cmd);

    } else if (picNum == 0) {
        PIO::outByte(PIC1_REG_COMMAND, cmd);
    }
}

/*
 * Function to write to the PIC'S Interrupt Mask Register
 * @cmd: the command byte to send
 * @picNum: which PIC to send the command to (0 for Master,
 * 1 for Slave)
 */
void PIC::writeIMR(uint8_t data, uint8_t picNum) {
    if (picNum == 1) {
        PIO::outByte(PIC2_REG_IMR, data);

    } else if (picNum == 0) {
        PIO::outByte(PIC1_REG_IMR, data);
    }
}

/*
 * Function to read from the PIC'S Data Register
 * @picNum: which PIC to read from (0 for Master, 1 for Slave)
 * @return: the byte that was received
 */
uint8_t PIC::readDR(uint8_t picNum) {
    uint8_t portNum;

    if (picNum == 1) {
        portNum = PIC2_REG_DATA;

    } else if (picNum == 0) {
        portNum = PIC1_REG_DATA;
    }

    return PIO::inByte(portNum);
}

/*
 * Function to read from the PIC'S portNum Register
 * @picNum: which PIC to read from (0 for Master, 1 for Slave)
 * @return: the byte that was received
 */
uint8_t PIC::readSR(uint8_t picNum) {
    uint8_t portNum;

    if (picNum == 1) {
        portNum = PIC2_REG_STATUS;

    } else if (picNum == 0) {
        portNum = PIC1_REG_STATUS;
    }

    return PIO::inByte(portNum);
}

