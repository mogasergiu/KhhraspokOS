#include <stdint.h>
#include <stddef.h>
#include <portsIO.hpp>
#include <acpi.hpp>

#ifndef INTERRUPTS_HPP
#define INTERRUPTS_HPP

#define MAX_IDT_ENTRIES 256
#define MAX_CPU_COUNT 16

#define APIC_ADDR 0x500

#define DIVISOR 100
#define INPUT_FREQ 1193180

/******************************************
 * x86 Hardware Interrupts for Master PIC *
 ******************************************/
#define	PIC1_IRQ_TIMER 0x0
#define	PIC1_IRQ_KEYBOARD 0x1
// IRQ 0x2 is reserved for Slave PIC communication
#define PIC1_IRQ_SLAVE_COMM 0x2
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
#define PIC2_IRQ_RESERVED_INT_2 0x2
#define PIC2_IRQ_RESERVED_INT_3 0x3
#define	PIC2_IRQ_AUXILIARY 0x4
#define	PIC2_IRQ_FPU 0x5
#define	PIC2_IRQ_HDDC 0x6
#define PIC2_IRQ_RESERVED_INT_7 0x7

// PIC End of Interrupt
#define PIC_EOI 0x20


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
#define PIC1_IRQ0  0x20
#define PIC2_IRQ8  0x28

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


/**************************
 * PIT Internal Registers - readable/writable *
 **************************/
#define PIT_REG_CHANNEL_0 0x40
#define PIT_REG_CHANNEL_1 0x41
#define PIT_REG_CHANNEL_2 0x42
#define PIT_REG_CW 0x43

/******************************
 * PIT Operation Common Words *
 ******************************/
// BCP bit - set to 1 for BCD Counter and Binary otherwise
#define PIT_OCW_BIN 0b0000000
#define PIT_OCW_BCP 0b0000001

// Operation mode 3 bits:
// 0b000 Mode 0: Interrupt or Terminal Count
#define PIT_OCW_MODE_TERMINALCOUNT 0b00000000
// 0b001 Mode 1: Programmable one-shot
#define PIT_OCW_MODE_ONESHOT 0b00000010
// 0b010 Mode 2: Rate Generator
#define PIT_OCW_MODE_RATEGENERATOR 0b00000100
// 0b011 Mode 3: Square Wave Generator
#define PIT_OCW_MODE_SQUAREWAVEGENERATOR 0b00000110
// 0b100 Mode 4: Software Triggered Strobe
#define PIT_OCW_MODE_SWTRIGGERED 0b00001000
// 0b101 Mode 5: Hardware Triggered Strobe
#define PIT_OCW_MODE_HWTRIGGERED 0b00001010
// 0b110 and 0b111 are the same as SWTRIGGERED and HWTRIGGERED

// Read/Load Mode 2 bits:
// 0b00 Latch count value command
#define PIT_OCW_RL_LATCH 0b00000000
// 0b01 Read/Load LSB only
#define PIT_OCW_RL_LSB 0b00010000
// 0b10 Read/Load MSB only
#define PIT_OCW_RL_MSB 0b00100000
// 0b11 Read/Load LSB first then MSB
#define PIT_OCW_RL_LMSB 0b00110000

// Select Channel 2 bits:
// 0b00 Channel 0
#define PIT_OCW_SC_0 0b00000000
// 0b01 Channel 1
#define PIT_OCW_SC_1 0b01000000
// 0b10 Channel 2
#define PIT_OCW_SC_2 0b10000000
// 0b11 READ-back command (8254 only)
#define PIT_OCW_SC_RB 0b11000000


/*********************************************
 * CLI and STI assembly instructions inlined *
 *********************************************/
#define cli()                                                                  \
    __asm__ __volatile__(                                                      \
        "cli"                                                                  \
        :                                                                      \
        :                                                                      \
    )

#define sti()                                                                  \
    __asm__ __volatile__(                                                      \
        "sti"                                                                  \
        :                                                                      \
        :                                                                      \
    )

/********************************************************************
 * INTERRUPTS namespace - covers every interrupts related component *
 ********************************************************************/
namespace INTERRUPTS {

    /*********************************************************************
     * This namespaces declares the IRQ Handlers whose implementation is *
     * located inside the pre-Kernel                                     *
     *********************************************************************/
    namespace IntHandlers {
        extern "C" void pitIRQHandler();
        extern "C" void keyboardIRQHandler();
        extern "C" void doNothingIRQHandler();
    }

    namespace IntCallbacks {
        /*
         * Callback function for Master PIC's Keyboard IRQ
         */
        extern "C" void keyboardIRQ();

        /*
         * Callback function for Master PIC's connection to the PIT
         */
        extern "C" void pitIRQ();
    };

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
                uint8_t zero8;

                // Contains P, DPL, S and Gate Type
                uint8_t typeAttr;

                // Higher part of the interrupt function's offset address
                uint16_t offset2;

                uint32_t offset3;

                uint32_t zero32;

            } __attribute__((packed)) IDTDescriptors[MAX_IDT_ENTRIES];

            // IDT Register descriptor structure
            struct IDTRD {
                // Defines the length of the IDT in bytes - 1
                uint16_t limit;

                // Linear address where the IDT starts (INT 0) 
                uint64_t base;
            } __attribute__((packed)) IDTRDescriptor;

            // Constructor - sets up IDTR, zeroes out IDT and loads IDT (lidt)
            void initInterrupts();

            /*
             * Helper function to add an entry in the IDT
             * @number: desired interrupt number of the entry
             * @handler: address of the handler to be placed in the entry
             */
            void setIDTEntry(uint32_t number, void (*handler)());

            /*
             * Helper function to retrieve an entry in the IDT
             * @number: desired interrupt number of the entry
             * @return: address of the function to be placed from the entry
             */
            void (*getIDTEntry)(uint32_t number);
    };

    /*********************************************************************
     * Class to describe the Programmable Interrupt Controller component *
     *********************************************************************/
    class PIC {
        public:
            // Constructor - sets up Programmable Interrupt Controller
            void initPIC();

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

    extern uint32_t ticks;

    /*********************************************************************
     * Class to describe the Programmable Interval Timer component *
     *********************************************************************/
    class PIT {
        public:
            // Constructor - Initializes the PIT
            void initPIT();

            /*
             * Getter method to retrieve the number of system ticks
             * @return: number of system ticks
             */
            uint32_t getTicks() const;

            /*
             * Write Operation Command Word to Command Register
             * @ocw: the OCW to send
             */
            void sendOCW(uint8_t ocw) const;

            /*
             * Get current count value from Channel Register
             * @channel: the Channel Register we get the value from
             * @return: value found in the Channel Register
             */
            uint8_t readChannel(uint8_t channel) const;

            /*
             * Set current count value of Channel Register
             * @channel: the Channel Register whose value we set
             * @value: the value we will set
             * @return: value found in the Channel Register
             */
            void sendChannel(uint8_t channel, uint8_t value) const;
    };

    class APIC {
        private:
            uint8_t lapicCount;
            uint8_t* lapicAddr;
            ACPI::LAPICEntry *lapic[MAX_CPU_COUNT];
            ACPI::IOAPICEntry *ioapic;
            uint8_t ioapicIntrSrcOverrideCount;
            ACPI::IOAPICIntrSrcOverride *ioapiciso[MAX_CPU_COUNT];
            uint8_t ioapicNmiSrcCount;
            ACPI::IOAPICNMISrc *ioapicns[MAX_CPU_COUNT];
            uint8_t lapicNmiCount;
            ACPI::LAPICNMI *lapicn[MAX_CPU_COUNT];
            ACPI::LAPICAddrOverride *lapicao;
            uint8_t lx2apicCount;
            ACPI::Lx2APIC *lx2apic[MAX_CPU_COUNT];

        public:
            void parseMADT();
    };
}

extern INTERRUPTS::Interrupts intsHandler;
extern INTERRUPTS::PIC picHandler;
extern INTERRUPTS::PIT pitHandler;

#endif  /* INTERRUPTS_HPP */

