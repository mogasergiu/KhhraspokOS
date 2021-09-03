#include <video.hpp>
#include <interrupts.hpp>
#include <string.hpp>
#include <kernel.hpp>
#include <kpkheap.hpp>
#include <memory.hpp>
#include <drivers.hpp>
#include <filesystem.hpp>
#include <acpi.hpp>

static inline void secondaryPICinit() {
    intsHandler.setIDTEntry(PIC1_IRQ0 + PIC1_IRQ_TIMER,
                            INTERRUPTS::IntHandlers::pitIRQHandler);
    intsHandler.setIDTEntry(PIC1_IRQ0 + PIC1_IRQ_KEYBOARD,
                            INTERRUPTS::IntHandlers::keyboardIRQHandler);
    for (int i = PIC1_IRQ0 + PIC1_IRQ_SERIAL2;
        i <= PIC1_IRQ0 + PIC1_IRQ_PARALLEL1; i++) {
        intsHandler.setIDTEntry(i, INTERRUPTS::IntHandlers::doNothingIRQHandler);
    }
    for (int i = PIC2_IRQ8; i <= PIC2_IRQ8 + PIC2_IRQ_HDDC; i++) {
        if (i == PIC2_IRQ8 + PIC2_IRQ_RESERVED_INT_2 ||
            i == PIC2_IRQ8 + PIC2_IRQ_RESERVED_INT_3) {
            continue;
        }

        intsHandler.setIDTEntry(i, INTERRUPTS::IntHandlers::doNothingIRQHandler);
    }

    for (int i = PIC2_IRQ8 + PIC2_IRQ_HDDC + 1; i < MAX_IDT_ENTRIES; i++) {
        intsHandler.setIDTEntry(i, INTERRUPTS::IntHandlers::doNothingIRQHandler);
    }
}

extern "C" void kernelMain() {
    intsHandler.initInterrupts();
    picHandler.initPIC();
    pitHandler.initPIT();
    secondaryPICinit();

    sti();

    ACPI::parseACPI();

    ACPI::madt->hdr.signature++;

    char *ptr = (char*)0xffff0000;
    ptr[0] = 'a';

    vgaHandler.putString("hello", 15);

    while (1) {};
}
