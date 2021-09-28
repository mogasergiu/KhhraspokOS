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
//        "hlt;"
        "sti;"
        :
        : "r" (&intsHandler.IDTRDescriptor)
        : "rsi"
    );

    while(1);
}

extern "C" void kernelMain() {
    GDT::createProperGDTs();
    GDT::loadProperGDT(0);

    intsHandler.initInterrupts();

    char shell[] = "/bin/shell";

    CATCH_FIRE(taskMgr.createTask(shell, 3, -1) < 0, "Could not spawn Shell!\n");
    taskMgr.createTask(&reaper, 0, NULL, taskMgr.getKernelPHdr());
    taskMgr.createTask(&loader, 0, NULL, taskMgr.getKernelPHdr());

    while (1);
}
