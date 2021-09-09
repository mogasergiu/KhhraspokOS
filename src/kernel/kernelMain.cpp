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
    apicHandler.LAPICout(LAPIC_SIVR, 0x100 | 0xff);

    apicHandler.LAPICout(LAPIC_LDR, 0x01000000);

    apicHandler.LAPICout(LAPIC_TPR, 0);

    apicHandler.LAPICout(LAPIC_DFR, 0xffffffff);

    apicHandler.initLAPICTimer();

    __asm__ __volatile__ (
        "movq %0, %%rsi;"
        "lidt 0(%%rsi);"
        "hlt"
        :
        : "r" (&intsHandler.IDTRDescriptor)
        : "rsi"
    );
}

extern "C" void kernelMain() { 
    intsHandler.initInterrupts();

    DRIVERS::PCI::printPCIDevices();

    char file[] = "/dir/2.txt";
    char mode[] = "r";
    fat16Handler.fopen(file, mode);

    while (1) {
        pitHandler.sleep(10);
        kprintf("CPUS: %d\n", *apicHandler.activeCPUs);
    };
}
