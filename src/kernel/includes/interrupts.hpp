#include <stdint.h>
#include <stddef.h>
#include <portsIO.hpp>

#ifndef INTERRUPTS_HPP
#define INTERRUPTS_HPP

#define MAX_IDT_ENTRIES 256

/******************************************
 * x86 Hardware Interrupts for Master PIC *
 ******************************************/
#define	PIC1_IRQ_TIMER 0x0
#define	PIC1_IRQ_KEYBOARD 0x1
// IRQ 0x2 is reserved for Slave PIC communication
#define	PIC1_IRQ_SERIAL2 0x3
#define	PIC1_IRQ_SERIAL1 0x4
#define	PIC1_IRQ_PARALLEL2 0x5
#define	PIC1_IRQ_DISKETTE 0x6
#define	PIC1_IRQ_PARALLEL1 0x7
 
/*****************************************
 * x86 Hardware Interrupts for Slave PIC *
 *****************************************/
#define	PIC2_IRQ_CMOSTIMER 0x0
#define	PIC2_IRQ_CGA 0x1
// IRQ'S 0x2 and 0x3 are reserved
#define	PIC2_IRQ_AUXILIARY 0x4
#define	PIC2_IRQ_FPU 0x5
#define	PIC2_IRQ_HDDC 0x6
// IRQ 0x7 is reserved

// PIC End of Interrupt
#define PIC_EOI                         0x20


#define PIC1 0x0  // Master PIC
#define PIC2 0x1  // Slave PIC


/*****************************************************
 * Port locations for the registers of the two PIC's *
 *****************************************************/
// Master PIC
#define PIC1_REG_COMMAND 0x20  // Write-only register
#define PIC1_REG_STATUS	0x20  // Read-only register
#define PIC1_REG_IMR 0x21  // Write-only register
#define PIC1_REG_DATA 0x21  // Read-only register
 
// Slave PIC
#define PIC2_REG_COMMAND 0xa0  // Write-only register
#define PIC2_REG_STATUS	0xa0  // Read-only register
#define PIC2_REG_IMR 0xa1  // Write-only register
#define PIC2_REG_DATA 0xa1  // Read-only register


/*******************************************************************
 * Initialization Control Words(ICW) used for initializing the PIC *
 *******************************************************************/
// ICW1 - primary control word
// IC4 bit - set to 1 if the PIC will expect an ICW4
#define PIC_ICW1_IC4 0b00000001
// SNGL bit - set to 1 if ionly one PIC (must send ICW3 otherwise)
#define PIC_ICW1_SNGL 0b00000010
// ADI bit - ignored by x86 - set to 0
#define PIC_ICW1_ADI 0b00000100
// LTIM bit - set to 1 if Level Triggered Mode (Edge Triggered Mode otherwise)
#define PIC_ICW1_LTIM 0b00001000
// INIT bit - set to 1 if PIC is to be initialized
#define PIC_ICW1_INIT 0b00010000

// ICW2 - used to map base address of IVT/IDT entries the PIC will take care of
#define PIC1_ICW2_IRQ0  0x20
#define PIC2_ICW2_IRQ8  0x28

// ICW3 - says how the PIC's are cascaded
// IRQ connection to Slave PIC is specified in binary in the case of Master PIC
#define PIC1_ICW3_PIC2_LINK 0b00000100
// IRQ connection to Master PIC is specified in decimal in the case of Slave PIC
#define PIC2_ICW3_PIC1_LINK 0x2

// ICW4 - says how interrupts are to be operated
// uPM bit - set to 1 if 80x86 mode, 0 if MCS-80/86 mode
#define PIC_ICW4_UPM 0b00000001
// AEOI bit - set to 1 if controller automatically performs End of Interrupt
#define PIC_ICW4_AEOI 0b00000010
// M/S bit - set to 1 if buffer master selected (buffer slave otherwise)
#define PIC_ICW4_MS 0b00000100
// BUF bit - set to 1 if controller operates in buffered mode
#define PIC_ICW4_BUF 0b00001000
// SFNM bit - set to 1 if Special Fully Nested Mode active (many cascades)
#define PIC_ICW4_SFNM 0b00010000


/*******************************
 * PIC Operation Command Words *
 *******************************/
// OCW1 is read from IMR
// OCW2 is the primary CW for PIC
// First three bits of OCW2 are the interrupt level
#define PIC_OCW3_L0 0b00000001
#define PIC_OCW3_L1 0b00000010
#define PIC_OCW3_L3 0b00000100
// bits 3 - 4 are reserved
#define PIC_OCW3_EOI 0b00100000  // End of Interrupt request
#define PIC_OCW3_SL 0b01000000  // Selection 
#define PIC_OCW3_R 0b10000000  // Rotation

/********************************************************************
 * INTERRUPTS namespace - covers every interrupts related component *
 ********************************************************************/
namespace INTERRUPTS {

    /*******************************************************************
     * Interrupts class is meant to act as a template and be inherited *
     *******************************************************************/
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

    /*********************************************************************
     * Class to describe the Programmable Interrupt Controller component *
     *********************************************************************/
    class PIC : public Interrupts {
        public:
            // Constructor - sets up Programmable Interrupt Controller
            PIC();

            /*
             * Function to write to the PIC'S Command Register
             * @cmd: the command byte to send
             * @picNum: which PIC to send the command to (0 for Master,
             * 1 for Slave)
             */
            void writeCMDR(uint8_t cmd, uint8_t picNum);

            /*
             * Function to write to the PIC'S Interrupt Mask Register
             * @cmd: the command byte to send
             * @picNum: which PIC to send the command to (0 for Master,
             * 1 for Slave)
             */
            void writeIMR(uint8_t data, uint8_t picNum);

            /*
             * Function to read from the PIC'S Data Register
             * @picNum: which PIC to read from (0 for Master, 1 for Slave)
             * @return: the byte that was received
             */
            uint8_t readDR(uint8_t picNum);

            /*
             * Function to read from the PIC'S Status Register
             * @picNum: which PIC to read from (0 for Master, 1 for Slave)
             * @return: the byte that was received
             */
            uint8_t readSR(uint8_t picNum);
    };
}

#endif  /* INTERRUPTS_HPP */

