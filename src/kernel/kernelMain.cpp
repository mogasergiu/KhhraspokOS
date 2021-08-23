#include <video.hpp>
#include <interrupts.hpp>
#include <string.hpp>
#include <kernel.hpp>
#include <kpkheap.hpp>
#include <paging.hpp>
#include <drivers.hpp>
#include <filesystem.hpp>

VIDEO::VGA::TextMode vgaHandler;
INTERRUPTS::Interrupts intsHandler;
INTERRUPTS::PIC picHandler;
INTERRUPTS::PIT pitHandler;
MMU::PgMgr pageManager;
FILESYSTEM::Path pathMgr;

void* KPKHEAP::topChunk = (void*)KPKHEAP_START;

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
}

static inline void secondaryPreKernel() {
    KPKHEAP::topChunk = (void*)KPKHEAP_START;
    DRIVERS::DISK::nail = 0;
}

extern "C" void kernelMain() {
    secondaryPreKernel();

    secondaryPICinit();

    vgaHandler.putString("hello", 15);

    FILESYSTEM::parsedPath *path = pathMgr.parsePath((char*)"/dev1/dev2/dev3");

    vgaHandler.putString(path->nextDir->prevDir->currDir, 15);
    vgaHandler.putString(path->nextDir->nextDir->prevDir->currDir, 15);

    if (path == NULL) {return;}

    while (1) {}
}
