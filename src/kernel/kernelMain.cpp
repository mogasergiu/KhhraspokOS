#include <video.hpp>
#include <interrupts.hpp>
#include <kstring.hpp>
#include <kernel.hpp>
#include <kpkheap.hpp>
#include <memory.hpp>
#include <drivers.hpp>
#include <filesystem.hpp>
#include <acpi.hpp>
#include <kstdio.hpp>

uint8_t cpuCount = 1;

extern "C" void APEntry() {
    __asm__ __volatile__("hlt"::);
}

extern "C" void kernelMain() { 
    intsHandler.initInterrupts();

    while (1) {
        pitHandler.sleep(20);
        kprintf("CPUS: %d\n", *apicHandler.activeCPUs);
    };
}
