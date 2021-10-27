#include <stddef.h>
#include <stdint.h>

#ifndef GDT_HPP
#define GDT_HPP

#define TOTAL_GDT_ENTRIES 0x5
#define NULL_SELECTOR 0x0
#define KERNEL_CODE 0x8
#define KERNEL_DATA 0x10
#define USER_DATA 0x1b
#define USER_CODE 0x23
#define TSS_SELECTOR 0x28

#define KERNEL_STACK_SIZE 0x1000

namespace GDT {
    struct EncodedEntry {
        uint32_t base;
        uint32_t limit;
        uint8_t type;
    } __attribute__((packed));

    // detailed description in src/boot/globalDescriptorTable.asm
    struct RealEntry {
        uint8_t limit0_15[2];
        uint8_t base0_23[3];
        uint8_t access;
        uint8_t flags : 4;
        uint8_t limit16_20 : 4;
        uint8_t base24_31;
    } __attribute__((packed));

    void encodeGDTEntry(uint8_t *target, EncodedEntry source);
    uint64_t createDescriptor(uint32_t base, uint32_t limit, uint16_t flag);

    struct TSS {
        uint32_t reserved0;
        uint64_t rsp0;
        uint64_t rsp1;
        uint64_t rsp2;
        uint64_t reserved1;
        uint64_t ist1;
        uint64_t ist2;
        uint64_t ist3;
        uint64_t ist4;
        uint64_t ist5;
        uint64_t ist6;
        uint64_t ist7;
        uint64_t reserved2;
        uint16_t reserved3;
        uint16_t iomapBase;
    } __attribute__((packed));

    void createProperGDTs();

    void loadProperGDT(uint8_t lapicID);

    struct Descriptor {
        uint16_t size;
        uint64_t addr;
    } __attribute__((packed));
}

extern GDT::EncodedEntry kernelEncodedGDT[];
extern GDT::EncodedEntry **perCpuGDT;
extern GDT::TSS **perCpuTSS;
extern GDT::RealEntry **perCpuRealGDT;
extern GDT::Descriptor *gdtDesc;

#endif  /*  GDT_HPP  */
