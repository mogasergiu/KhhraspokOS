#include <video.hpp>
#include <interrupts.hpp>
#include <string.hpp>
#include <kernel.hpp>
#include <kpkheap.hpp>
#include <paging.hpp>

VIDEO::VGA::TextMode vgaHandler;
INTERRUPTS::Interrupts intsHandler;
INTERRUPTS::PIC picHandler;
INTERRUPTS::PIT pitHandler;
MMU::PgMgr pageManager;

void* KPKHEAP::topChunk = (void*)KPKHEAP_START;

extern "C" void kernelMain() {
    KPKHEAP::topChunk = (void*)KPKHEAP_START;

    intsHandler.setIDTEntry(PIC1_IRQ0 + PIC1_IRQ_TIMER,
                            INTERRUPTS::IntHandlers::pitIRQHandler);
    intsHandler.setIDTEntry(PIC1_IRQ0 + PIC1_IRQ_KEYBOARD,
                            INTERRUPTS::IntHandlers::keyboardIRQHandler);

    vgaHandler.putString("hello", 15);

    __asm__ __volatile__("hlt");
}
