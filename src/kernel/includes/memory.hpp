#include <stdint.h>
#include <stddef.h>
#include <string.hpp>
#include <ds.hpp>

#ifndef PAGING_HPP
#define PAGING_HPP

#define MEM_REGION_COUNT_ADDR 0x500
#define MEM_MAP_ADDR 0x510

#define KERNEL_USED_MEM 0x400000
#define KERNEL_START_ADDR 0x100000

#define START_MEM 0x10000000

#define PDE_G 0b100000000
#define PDE_S 0b010000000
#define PDE_A 0b000100000
#define PDE_D 0b000010000
#define PDE_W 0b000001000
#define PDE_U 0b000000100
#define PDE_R 0b000000010
#define PDE_P 0b000000001
#define PDE_NX (1 << 63)

#define PTE_G 0b100000000
#define PTE_D 0b001000000
#define PTE_A 0b000100000
#define PTE_C 0b000010000
#define PTE_W 0b000001000
#define PTE_U 0b000000100
#define PTE_R 0b000000010
#define PTE_P 0b000000001
#define PTE_NX (1 << 63)

#define PML4T_ADDR 0x1000
#define PDPT0_aDDR 0x2000
#define PDT0_ADDR 0x3000
#define PT0_ADDR 0x4000

#define P_TOTAL_ENTRIES 512
#define PAGE_SIZE 4096

#define PTidx(vaddr) ((((uintptr_t)vaddr) >> 12) & 0x1ff)
#define PDidx(vaddr) ((((uintptr_t)vaddr) >> 21) & 0x1ff)
#define PDTidx(vaddr) ((((uintptr_t)vaddr) >> 30) & 0x1ff)
#define PML4idx(vaddr) ((((uintptr_t)vaddr) >> 39) & 0x1ff)

#define USABLE_MEM 0x1
#define RESERVED_MEM 0x2
#define ACPI_RECLAIMABLE_MEM 0x3
#define ACPI_NVS_MEM 0x4
#define BAD_MEM 0x5

#define setPgFlag(pg, flags) (pg = (pgTbl*)(((uintptr_t)pg | flags)))
#define getPgFlag(pg, flag) ((uintptr_t)pg & (1 << flag))
#define getPgAddr(pg) (pgTbl*)((((uintptr_t)pg >> (uintptr_t)12) << (uintptr_t)12) & (uintptr_t)0xffffffffff000)
#define setPgAddr(pg, paddr) (pg = (pgTbl*)(((uintptr_t)pg & (uintptr_t)0xfff0000000000fff) | \
                             ((((uintptr_t)paddr >> (uintptr_t)12) & (uintptr_t)0xffffffffff) << (uintptr_t)12)))

namespace MMU {

    extern uint8_t memRegionCount;
    extern size_t freeMem;
    extern size_t usedMem;
    extern size_t reservedMem;

    struct memRegionDescriptor {
        uint64_t baseAddr;
        uint64_t length;
        uint32_t type;
        uint32_t attr;
    } __attribute__((packed));

    struct pgTbl {
        uint64_t *entries[P_TOTAL_ENTRIES];
    }__attribute__((aligned(PAGE_SIZE)));

    extern memRegionDescriptor *memMap;

    class PgFrAllocator {
        private:
            BitMap *PgsMap;
            size_t bitmapIdx;

        public:
            void initPgAlloc();
            void freePg(void* addr);
            void freePgs(void* addr, size_t count);
            void allocPg(void* addr);
            void allocPgs(void* addr, size_t count);
            void* reqPg();
    }; 

    class PgMgr {
        private:
            static PgFrAllocator pgAllocator;

            struct PML4struct {
                pgTbl *entries[4];
            } __attribute__((packed)) *PML4;

        public:
            PgMgr();

            void mapPg(void *vaddr, void *paddr);
    };
}

#endif /* PAGING_HPP */