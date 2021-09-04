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
    apicHandler.parseMADT();

    intsHandler.initInterrupts();

    sti();

    CATCH_FIRE(1, "END!");

    while (1) {};
}
