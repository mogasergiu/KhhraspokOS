#include <video.hpp>
#include <interrupts.hpp>
#include <string.hpp>
#include <kernel.hpp>
#include <kpkheap.hpp>
#include <memory.hpp>
#include <drivers.hpp>
#include <filesystem.hpp>
#include <acpi.hpp>
#include <stdio.hpp>

extern "C" void kernelMain() {
    intsHandler.initInterrupts();

    sti();

    ACPI::parseACPI();

    ACPI::madt->hdr.signature++;

    char *ptr = (char*)0xffff0000;
    ptr[0] = 'a';

    vgaHandler.putString("hello", 15);
    printf("%s\n%s", "hello", "hello");
    pwarn("ERROR!");
    putc('a');

    printf("%x.%x.%x\n", ptr, 1234, 0xff431ad);

    CATCH_FIRE(1, "END!");

    apicHandler.parseMADT();

    while (1) {};
}
