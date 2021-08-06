#include <video.hpp>
#include <interrupts.hpp>
#include <string.hpp>
#include <kernel.hpp>

VIDEO::VGA::TextMode vga;
INTERRUPTS::PIC pic;
INTERRUPTS::PIT pit;

static void handleZero() {
    vga.putString("zerooo", 15);
}

extern "C" void kernelMain() {
    vga.putString("hello", 15);
    pit.getTicks();
    pit.getTicks();
    pit.getTicks();
    pit.getTicks();
    pit.getTicks();
    pit.getTicks();
    pit.getTicks();
    pit.getTicks();
    pit.getTicks();
    pit.getTicks();
    pit.getTicks();
    pit.getTicks();
    pit.getTicks();
    pit.getTicks();
    pit.getTicks();
    pit.getTicks();
    // __asm__ __volatile__ ("int $0x0"::);
}
