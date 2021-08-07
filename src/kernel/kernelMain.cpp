#include <video.hpp>
#include <interrupts.hpp>
#include <string.hpp>
#include <kernel.hpp>

VIDEO::VGA::TextMode vgaHandler;
INTERRUPTS::Interrupts intsHandler;
INTERRUPTS::PIC picHandler;
INTERRUPTS::PIT pitHandler;

extern "C" void kernelMain() {
    intsHandler.setIDTEntry(PIC1_IRQ0 + PIC1_IRQ_TIMER,
                            INTERRUPTS::IntHandlers::pitIRQHandler);
    intsHandler.setIDTEntry(PIC1_IRQ0 + PIC1_IRQ_KEYBOARD,
                            INTERRUPTS::IntHandlers::keyboardIRQHandler);
    vgaHandler.putString("hello", 15);
    __asm__ __volatile__ ("int $0x21"::);
    while(1);
}
