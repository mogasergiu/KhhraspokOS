#include <interrupts.hpp>
#include <kstdio.hpp>

using namespace INTERRUPTS;

INTERRUPTS::Interrupts intsHandler;

void APIC::initLAPICTimer() {
    static uint32_t ticks;

    if (ticks == 0) {
        apicHandler.LAPICout(LAPIC_TDCR, 0x3);

        apicHandler.LAPICout(LAPIC_TICR, 0xffffffff);

        apicHandler.LAPICout(LAPIC_LTR, LAPIC_TIMER_IDT_ENTRY);

        pitHandler.sleep(10);

        apicHandler.LAPICout(LAPIC_LTR, LAPIC_TIMER_DISABLE);

        ticks = 0xffffffff - apicHandler.LAPICin(LAPIC_TCCR);
    }


    apicHandler.LAPICout(LAPIC_LTR, LAPIC_TIMER_IDT_ENTRY |
                                    LAPIC_TIMER_PERIODIC_MODE);

    apicHandler.LAPICout(LAPIC_TDCR, 0x3);

    apicHandler.LAPICout(LAPIC_TICR, ticks / 100);
}

static void wakeAPs() {
    for (uint8_t i = 0, id = apicHandler.getLAPICID();
        i < apicHandler.getLAPICCount(); i++) {
        uint8_t _id = apicHandler.getLAPICID(i);

        if (id != _id) {
            apicHandler.sendLAPICIPI(_id, LAPIC_ICR_INIT |
                                          LAPIC_ICR_PHYSICAL |
                                          LAPIC_ICR_ASSERT |
                                          LAPIC_ICR_EDGE |
                                          LAPIC_ICR_DEFAULT);
        }
    }

    pitHandler.sleep(10);

    for (uint8_t i = 0, id = apicHandler.getLAPICID();
        i < apicHandler.getLAPICCount(); i++) {
        uint8_t _id = apicHandler.getLAPICID(i);

        if (id != _id) {
            apicHandler.sendLAPICIPI(_id, 0x8 | LAPIC_ICR_SIPI |
                                          LAPIC_ICR_PHYSICAL |
                                          LAPIC_ICR_ASSERT |
                                          LAPIC_ICR_EDGE |
                                          LAPIC_ICR_DEFAULT);
        }
    }
}

// Constructor - sets up IDTR, zeroes out IDT and loads IDT (lidt)
void Interrupts::initInterrupts() {
    picHandler.initPIC();
    apicHandler.initAPICs();
    pitHandler.initPIT();

    for (uint32_t i = 0; i < sizeof(this->IDTDescriptors); i++) {
        ((char*)this->IDTDescriptors)[i] = 0;
    }

    this->IDTRDescriptor.limit = sizeof(this->IDTDescriptors) - 1;
    this->IDTRDescriptor.base = (uintptr_t) this->IDTDescriptors;

    uintptr_t *idtd = (uintptr_t*)IDTD_ADDR;
    *idtd = (uintptr_t)(&this->IDTRDescriptor);

    for (int i = 0; i < MAX_IDT_ENTRIES; i++) {
        this->setIDTEntry(i, IntHandlers::doNothingIRQHandler);
    }

    this->setIDTEntry(PIC1_IRQ0 + PIC1_IRQ_TIMER, IntHandlers::pitIRQHandler);
    this->setIDTEntry(PIC1_IRQ0 + PIC1_IRQ_KEYBOARD,
                        IntHandlers::keyboardIRQHandler);
    this->setIDTEntry(LAPIC_TIMER_IDT_ENTRY, IntHandlers::lapicTimerIRQHandler);
    this->setIDTEntry(SPURIOUS_INTERRUPT_IDT_ENTRY,
                            IntHandlers::SpuriousInterruptHandler);

    __asm__ __volatile__ (
        "movq %0, %%rsi;"
        "lidt 0(%%rsi);"
        :
        : "r" (&this->IDTRDescriptor)
        : "rsi"
    );

    wakeAPs();
    apicHandler.initLAPICTimer();

    sti();
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
    descriptor->offset1 = (uintptr_t)address & 0x0000ffff;

    // Code Segment from GDT
    descriptor->selector = 0x8;

    // Must always be zero
    descriptor->zero8 = 0x0;

    // P = 1b, DPL = 00b, S = 0b, type = 1110b
    descriptor->typeAttr = 0x8E;

    // Higher part of the interrupt function's offset address
    descriptor->offset2 = ((uintptr_t)address & 0xffff0000) >> 0x10;

    descriptor->offset3 = ((uintptr_t)address & 0xffffffff00000000) >> 0x20;

    descriptor->zero32 = 0x0;

    sti();
}

