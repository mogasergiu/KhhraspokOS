#include <stdint.h>
#include <stddef.h>
#include <io.hpp>
#include <acpi.hpp>

#ifndef INTERRUPTS_HPP
#define INTERRUPTS_HPP

#define MAX_IDT_ENTRIES 256
#define MAX_CPU_COUNT 16

#define APIC_ADDR 0x500
#define IDTD_ADDR 0x50a

#define DIVISOR 100
#define INPUT_FREQ 1193180

#define SPURIOUS_INTERRUPT_IDT_ENTRY 0xff

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

#define IOREGSEL 0x00
#define IOWIN 0x10
#define IOAPICID 0x00
#define IOAPICVER 0x01
#define IOAPICARB 0x02
#define IOREDTBL 0x10

#define LAPIC_ID 0x0020
#define LAPIC_VERSION 0x0030
#define LAPIC_TPR 0x0080
#define LAPIC_APR 0x0090
#define LAPIC_PPR 0x00a0
#define LAPIC_EOI 0x00b0
#define LAPIC_RRD 0x00c0
#define LAPIC_LDR 0x00d0
#define LAPIC_DFR 0x00e0
#define LAPIC_SIVR 0x00f0
#define LAPIC_ISR 0x0100
#define LAPIC_TMR 0x0180
#define LAPIC_IRR 0x0200
#define LAPIC_ESR 0x0280
#define LAPIC_CMCI 0x02f0
#define LAPIC_ICR0 0x0300
#define LAPIC_ICR1 0x0310
#define LAPIC_LTR 0x0320
#define LAPIC_TSR 0x0330
#define LAPIC_PMCR 0x0340
#define LAPIC_LINT0 0x0350
#define LAPIC_LINT1 0x0360
#define LAPIC_LINTERR 0x0370
#define LAPIC_TICR 0x0380
#define LAPIC_TCCR 0x0390
#define LAPIC_TDCR 0x03e0
#define LAPIC_ICR_NORMAL 0x00000000
#define LAPIC_ICR_LOWEST 0x00000100
#define LAPIC_ICR_SMI 0x00000200
#define LAPIC_ICR_NMI 0x00000400
#define LAPIC_ICR_INIT 0x00000500
#define LAPIC_ICR_SIPI 0x00000600
#define LAPIC_ICR_PHYSICAL 0x00000000
#define LAPIC_ICR_LOGICAL 0x00000800
#define LAPIC_ICR_IDLE 0x00000000
#define LAPIC_ICR_PENDING 0x00001000
#define LAPIC_ICR_DEASSERT 0x00000000
#define LAPIC_ICR_ASSERT 0x00004000
#define LAPIC_ICR_EDGE 0x00000000
#define LAPIC_ICR_LEVEL 0x00008000
#define LAPIC_ICR_DEFAULT 0x00000000
#define LAPIC_ICR_ITSELF 0x00040000
#define LAPIC_ICR_ALL_CPUS 0x00080000
#define LAPIC_ICR_ALL_CPUS_BUT_ITSELF 0x000c0000

#define LAPIC_TIMER_IDT_ENTRY 0x22
#define LAPIC_TIMER_DISABLE 0x10000
#define LAPIC_TIMER_PERIODIC_MODE 0x20000

#define INT_DEFAULT 0x0
#define INT_NMI 0x1
#define INT_SYSCALL 0x2

#define SYSCALL_IDT_ENTRY 0x80

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

namespace INTERRUPTS {

    namespace IntHandlers {
        extern "C" void pitIRQHandler();
        extern "C" void syscallISRHandler();
        extern "C" void keyboardIRQHandler();
        extern "C" void lapicTimerIRQHandler();
        extern "C" void doNothingIRQHandler();
        extern "C" void SpuriousInterruptHandler();
        extern "C" void dumpCPU();
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

        extern "C" void lapicTimerIRQ();

        extern "C" long syscallISR(long arg1, ...);
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
                uint8_t ist;

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
             */
            void setIDTEntry(uint32_t number, void (*handler)(), uint8_t type);

            /*
             * Helper function to retrieve an entry in the IDT
             */
            void (*getIDTEntry)(uint32_t number);
    };

    class PIC {
        public:
            // Constructor - sets up Programmable Interrupt Controller
            void initPIC();

            /*
             * Function to write to the PIC'S Command Register
             */
            void writeCMDR(uint8_t cmd, uint8_t picNum);

            /*
             * Function to write to the PIC'S Interrupt Mask Register
             */
            void writeIMR(uint8_t data, uint8_t picNum);

            /*
             * Function to read from the PIC'S Data Register
             */
            uint8_t readDR(uint8_t picNum);

            /*
             * Function to read from the PIC'S Status Register
             */
            uint8_t readSR(uint8_t picNum);
    };

    extern uint32_t ticks;

    class PIT {
        public:
            // Constructor - Initializes the PIT
            void initPIT();

            void sleep(uint64_t ms30) const;

            /*
             * Getter method to retrieve the number of system ticks
             */
            uint32_t getTicks() const;

            /*
             * Write Operation Command Word to Command Register
             */
            void sendOCW(uint8_t ocw) const;

            /*
             * Get current count value from Channel Register
             */
            uint8_t readChannel(uint8_t channel) const;

            /*
             * Set current count value of Channel Register
             */
            void sendChannel(uint8_t channel, uint8_t value) const;
    };

    class APIC {
        private:
            ACPI::IOAPIC *ioapic;
            uint8_t ioapicisoCount;
            ACPI::IOAPICIntrSrcOverride *ioapiciso[MAX_CPU_COUNT];
            uint8_t ioapicnsCount;
            ACPI::IOAPICNMISrc *ioapicns[MAX_CPU_COUNT];
            uint8_t lapicnCount;
            ACPI::LAPICNMI *lapicn[MAX_CPU_COUNT];
            ACPI::LAPICAddrOverride *lapicao;
            uint8_t lx2apicCount;
            ACPI::Lx2APIC *lx2apic[MAX_CPU_COUNT];
            ACPI::LAPIC *lapic[MAX_CPU_COUNT];
            uintptr_t *lapicAddr;
            uint8_t lapicCount;

        public:
            uint8_t *activeCPUs;
            APIC();
            void initLAPICTimer();
            uint8_t getLAPICCount();
            void IOAPICout(uint8_t reg, uint32_t value);
            uint32_t IOAPICin(uint8_t reg);
            void LAPICout(uint32_t reg, uint32_t value);
            uint32_t LAPICin(uint32_t reg);
            void initAPICs();
            void setIOAPICEntry(uint8_t idx, uintptr_t addr);
            void parseMADT();
            uint8_t getLAPICID();
            uint8_t getLAPICID(uint8_t id);
            void sendLAPICEOI();
            void sendLAPICIPI(uint8_t remoteLAPICID, uint32_t flags);
            uint8_t getIOAPICVersion();
            uint8_t getIOAPICEntriesCount();
    };
}

extern INTERRUPTS::Interrupts intsHandler;
extern INTERRUPTS::PIC picHandler;
extern INTERRUPTS::PIT pitHandler;
extern INTERRUPTS::APIC apicHandler;

#endif  /* INTERRUPTS_HPP */

