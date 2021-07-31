#include <stdint.h>
#include <stddef.h>

#ifndef INTERRUPTS_HPP
#define INTERRUPTS_HPP

#define MAX_IDT_ENTRIES 256

// INTERRUPTS namespace - covers every interrupts related component
namespace INTERRUPTS {

    // Interrupts class is meant to act as a template and be inherited
    class Interrupts {
        public:

            // IDT entry structure
            struct IDTD {

                // Lower part of the interrupt function's offset address
                uint16_t offset1;

                // Points to a valid descriptor in the GDT (Code Segment)
                uint16_t selector;

                // Always zero
                uint8_t zero;

                // Contains P, DPL, S and Gate Type
                uint8_t typeAttr;

                // Higher part of the interrupt function's offset address
                uint16_t offset2;
            } __attribute__((packed)) IDTDescriptors[MAX_IDT_ENTRIES];

            // IDT Register descriptor structure
            struct IDTRD {
                // Defines the length of the IDT in bytes - 1
                uint16_t limit;

                // Linear address where the IDT starts (INT 0) 
                uint32_t base;
            } __attribute__((packed)) IDTRDescriptor;

            // Constructor - sets up IDTR, zeroes out IDT and loads IDT (lidt)
            Interrupts();

            /*
             * Helper function to add an entry in the IDT
             * @number: desired interrupt number of the entry
             * @address: address of the function to be placed in the entry
             */
            void setIDTEntry(uint32_t number, void (*address)());

            /*
             * Helper function to retrieve an entry in the IDT
             * @number: desired interrupt number of the entry
             * @return: address of the function to be placed from the entry
             */
            void (*getIDTEntry)(uint32_t number);
    };

    void handleZero();

    // Class to describe the Programmable Interrupt Controller component
    class PIC : public Interrupts {
        public:
            PIC();
    };
}

#endif  /* INTERRUPTS_HPP */

