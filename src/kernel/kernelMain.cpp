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
#include <kstdlib.hpp>
#include <gdt.hpp>
#include <kdsa.hpp>
#include <task.hpp>

extern "C" void APEntry() {
    GDT::loadProperGDT(apicHandler.getLAPICID());

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
    GDT::createProperGDTs();
    GDT::loadProperGDT(0);

    intsHandler.initInterrupts();

    DRIVERS::PCI::printPCIDevices();

    char file1[] = "/1.txt";
    char mode[] = "r";
    int fd = kfopen(file1, mode);
    char buffer[30000];
    kfread(fd, buffer, sizeof(buffer));

    char file2[] = "/dir/2.txt";
    fd = kfopen(file2, mode);
    kfread(fd, buffer, sizeof(buffer));

    kprintf("%x\n", MMU::userPD);

    char file3[] = "/dir/2.txt";
    taskMgr.loadTask(file3);

    while (1) {
        pitHandler.sleep(10);
        kprintf("CPUS: %d\n", *apicHandler.activeCPUs);
    };
}
