#include <video.hpp>
#include <interrupts.hpp>
#include <string.hpp>
#include <kernel.hpp>
#include <kpkheap.hpp>

VIDEO::VGA::TextMode vgaHandler;
INTERRUPTS::Interrupts intsHandler;
INTERRUPTS::PIC picHandler;
INTERRUPTS::PIT pitHandler;

void* KPKHEAP::topChunk = (void*)KPKHEAP_START;

extern "C" void kernelMain() {
    intsHandler.setIDTEntry(PIC1_IRQ0 + PIC1_IRQ_TIMER,
                            INTERRUPTS::IntHandlers::pitIRQHandler);
    intsHandler.setIDTEntry(PIC1_IRQ0 + PIC1_IRQ_KEYBOARD,
                            INTERRUPTS::IntHandlers::keyboardIRQHandler);
    vgaHandler.putString("hello", 15);
    __asm__ __volatile__ ("int $0x21"::);
    void *p1, *p2, *p3;
    KPKHEAP::topChunk = (void*)KPKHEAP_START;
    while(1) {
        p1 = KPKHEAP::kpkMalloc(100);
        p2 = KPKHEAP::kpkMalloc(10);
        p3 = KPKHEAP::kpkMalloc(80);
        KPKHEAP::kpkFree(p1);
        KPKHEAP::kpkFree(p2);
        KPKHEAP::kpkFree(p3);
        p1 = KPKHEAP::kpkMalloc(100);
        p2 = KPKHEAP::kpkMalloc(10);
        p3 = KPKHEAP::kpkMalloc(80);
        KPKHEAP::kpkFree(p1);
    }
}
