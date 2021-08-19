#include <stdint.h>
#include <stddef.h>
#include <string.hpp>

#ifndef PAGING_HPP
#define PAGING_HPP

#define PDE_G 0b100000000
#define PDE_S 0b010000000
#define PDE_A 0b000100000
#define PDE_D 0b000010000
#define PDE_W 0b000001000
#define PDE_U 0b000000100
#define PDE_R 0b000000010
#define PDE_P 0b000000001

#define PTE_G 0b100000000
#define PTE_D 0b001000000
#define PTE_A 0b000100000
#define PTE_C 0b000010000
#define PTE_W 0b000001000
#define PTE_U 0b000000100
#define PTE_R 0b000000010
#define PTE_P 0b000000001

#define PDE_ENTRIES_ADDR 0x900000
#define PTE_ENTRIES_ADDR 0xa00000

#define PDE_TOTAL_ENTRIES 1024
#define PTE_TOTAL_ENTRIES 1024
#define PAGE_SIZE 4096

namespace MMU {

    class PgMgr {
        private:
            uint32_t *PDE, *currPDE, *currPTE;

        public:
            PgMgr();

            uint32_t getPDE();

            void mapPage(void *vaddr, void *paddr);
    };
}

#endif /* PAGING_HPP */
