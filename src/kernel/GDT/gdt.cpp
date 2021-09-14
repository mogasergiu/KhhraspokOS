#include <gdt.hpp>
#include <kpkheap.hpp>
#include <kstdio.hpp>
#include <kstring.hpp>
#include <interrupts.hpp>

using namespace GDT;

GDT::EncodedEntry **perCpuGDT = NULL;
GDT::TSS **perCpuTSS = NULL;
GDT::RealEntry **perCpuRealGDT = NULL;
GDT::Descriptor *gdtDesc = NULL;

static inline void* align8(void *addr) {
    uintptr_t _addr = (uintptr_t)addr;
    uintptr_t remainder = _addr % 8;
    return (void*)(remainder ? (uint8_t*)addr + (8 - remainder) : addr);
}

EncodedEntry kernelEncodedGDT[] = {
    {.base = 0x00, .limit = 0x00, .type = 0x00},  // NULL Segment
    {.base = 0x00, .limit = 0xffffffff, .type = 0x9a}, // Kernel code segment
    {.base = 0x00, .limit = 0xffffffff, .type = 0x92}, // Kernel data segment
    {.base = 0x00, .limit = 0xffffffff, .type = 0xfa},  // User code segment
    {.base = 0x00, .limit = 0xffffffff, .type = 0xf2}  // User data segment
};

void GDT::encodeGDTEntry(uint8_t *target, EncodedEntry source) {
    // Check the limit to make sure that it can be Encoded
    if ((source.limit > 65536) && ((source.limit & 0xFFF) != 0xFFF)) {
        CATCH_FIRE(1, "You can't do that!");
    }
    if (source.limit > 65536) {
        // Adjust granularity if required
        source.limit = source.limit >> 12;
        target[6] = 0xa0;

    } else {
        target[6] = 0x20;
    }
 
    // Encode the limit
    target[0] = source.limit & 0xFF;
    target[1] = (source.limit >> 8) & 0xFF;
    target[6] |= (source.limit >> 16) & 0xF;
 
    // Encode the base 
    target[2] = source.base & 0xFF;
    target[3] = (source.base >> 8) & 0xFF;
    target[4] = (source.base >> 16) & 0xFF;
    target[7] = (source.base >> 24) & 0xFF;
 
    // And... Type
    target[5] = source.type;
}

uint64_t GDT::createDescriptor(uint32_t base, uint32_t limit, uint16_t flag) {
    uint64_t descriptor;
 
    // Create the high 32 bit segment
    descriptor  =  limit       & 0x000F0000;  // set limit bits 19:16
    descriptor |= (flag <<  8) & 0x00F0FF00;  // set type, p, dpl, s, g, d/b, l and avl fields
    descriptor |= (base >> 16) & 0x000000FF;  // set base bits 23:16
    descriptor |=  base        & 0xFF000000;  // set base bits 31:24
 
    // Shift by 32 to allow for low part of segment
    descriptor <<= 32;
 
    // Create the low 32 bit segment
    descriptor |= base  << 16;  // set base bits 15:0
    descriptor |= limit  & 0x0000FFFF;  // set limit bits 15:0

    return descriptor;
}

void GDT::createProperGDTs() {
    if (perCpuGDT == NULL) {
        perCpuGDT = (EncodedEntry**)KPKHEAP::kpkZalloc(MAX_CPU_COUNT *
                                                        sizeof(*perCpuGDT));
        CATCH_FIRE(perCpuGDT == NULL, "Could not allocate per CPU GDT array");

        for (int i = 0; i < MAX_CPU_COUNT; i++) {
            perCpuGDT[i] = (EncodedEntry*)KPKHEAP::kpkZalloc((TOTAL_GDT_ENTRIES + 1)
                                                        * sizeof(*perCpuGDT[i]));

            CATCH_FIRE(perCpuGDT[i] == NULL,
                        "Could not allocate per CPU GDT array");

            memcpy(perCpuGDT[i], &kernelEncodedGDT, sizeof(kernelEncodedGDT));
        }
    }

    if (perCpuTSS == NULL) {
        perCpuTSS = (TSS**)KPKHEAP::kpkZalloc(MAX_CPU_COUNT *
                                            sizeof(*perCpuTSS));
        CATCH_FIRE(perCpuTSS == NULL, "Could not allocate per CPU TSS");


        for (int i = 0; i < MAX_CPU_COUNT; i++) {
            perCpuTSS[i] = (TSS*)KPKHEAP::kpkZalloc((sizeof(*perCpuTSS[i])));

            CATCH_FIRE(perCpuTSS[i] == NULL, "Could not allocate per CPU TSS");

            perCpuTSS[i]->rsp0 = (uint64_t)align8(KPKHEAP::kpkZalloc(KERNEL_STACK_SIZE));
            CATCH_FIRE(perCpuTSS[i]->rsp0 == 0,
                    "Could not allocate per CPU Kernel Stack");
            perCpuTSS[i]->rsp0 += KERNEL_STACK_SIZE;

            perCpuTSS[i]->ist1 = (uint64_t)align8(KPKHEAP::kpkZalloc(KERNEL_STACK_SIZE));
            CATCH_FIRE(perCpuTSS[i]->ist1 == 0,
                    "Could not allocate per CPU Default Interrupt Stack");
            perCpuTSS[i]->ist1 += KERNEL_STACK_SIZE;

            perCpuTSS[i]->ist2 = (uint64_t)align8(KPKHEAP::kpkZalloc(KERNEL_STACK_SIZE));
            CATCH_FIRE(perCpuTSS[i]->ist2 == 0,
                    "Could not allocate per CPU Non-Maskable Interrupt Stack");
            perCpuTSS[i]->ist2 += KERNEL_STACK_SIZE;

            perCpuTSS[i]->ist3 = (uint64_t)align8(KPKHEAP::kpkZalloc(KERNEL_STACK_SIZE));
            CATCH_FIRE(perCpuTSS[i]->ist3 == 0,
                        "Could not allocate per CPU System Call Stack");
            perCpuTSS[i]->ist3 += KERNEL_STACK_SIZE;

            perCpuGDT[i][TOTAL_GDT_ENTRIES].base = (uint32_t)((uintptr_t)perCpuTSS[i]);
            perCpuGDT[i][TOTAL_GDT_ENTRIES].limit = sizeof(*perCpuTSS[i]) - 1;
            perCpuGDT[i][TOTAL_GDT_ENTRIES].type = 0x89;
        }
    }

    if (perCpuRealGDT == NULL) {
        perCpuRealGDT = (RealEntry**)KPKHEAP::kpkZalloc(MAX_CPU_COUNT
                                                        * sizeof(*perCpuRealGDT));
        CATCH_FIRE(perCpuRealGDT == NULL,
                    "Could not allocate per CPU Real GDT array");

        for (int i = 0; i < MAX_CPU_COUNT; i++) {
            perCpuRealGDT[i] = (RealEntry*)KPKHEAP::kpkZalloc(
                                (TOTAL_GDT_ENTRIES + 2) *
                                sizeof(*perCpuRealGDT[i]));

            CATCH_FIRE(perCpuRealGDT[i] == NULL,
                        "Could not allocate per Real CPU GDT array");

            for (int j = 1; j < TOTAL_GDT_ENTRIES + 1; j++) {
                encodeGDTEntry((uint8_t*)&perCpuRealGDT[i][j], perCpuGDT[i][j]);
            }
        }
    }

   if (gdtDesc == NULL) {
        gdtDesc = (Descriptor*)KPKHEAP::kpkZalloc(MAX_CPU_COUNT
                                                    * sizeof(*gdtDesc));
        CATCH_FIRE(perCpuRealGDT == NULL, "Could not allocate per CPU GDTD");

        for (int i = 0; i < TOTAL_GDT_ENTRIES + 1; i++) {
            gdtDesc[i].size = (TOTAL_GDT_ENTRIES + 2) * sizeof(**perCpuRealGDT);
            gdtDesc[i].size--;

            gdtDesc[i].addr = (uint64_t)((uintptr_t)perCpuRealGDT[i]);
        }
    }
}

void GDT::loadProperGDT(uint8_t lapicID) {
    CATCH_FIRE(perCpuGDT[lapicID] == NULL, "This AP does not have a GDT!");

    __asm__ __volatile__ (
        "movq %0, %%r15;"
        "lgdt 0(%%r15);"
        "ltr %1;"
        :
        : "r" (&gdtDesc[lapicID]), "r" ((uint16_t)TSS_SELECTOR)
        : "r15"
    );
}
