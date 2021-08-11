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

    // this->setIDTEntry(PIC1_IRQ0 + PIC1_IRQ_TIMER, pitIRQ);
}

/*
 * Helper function to add an entry in the IDT
 * @number: desired interrupt number of the entry
 * @address: address of the function to be placed in the entry
 */
void Interrupts::setIDTEntry(uint32_t number, void (*address)()) {
    cli();

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

    sti();
}

