#include <paging.hpp>
#include <kpkheap.hpp>
#include <video.hpp>

using namespace MMU;

PgMgr::PgMgr() {
    this->PDE = (uint32_t*)PDE_ENTRIES_ADDR;

    int startRange = 0;
    uint32_t *PTE = (uint32_t*)PTE_ENTRIES_ADDR;

    for (int pde = 0; pde < PDE_TOTAL_ENTRIES; pde++) {

        for (int pte = 0; pte < PTE_TOTAL_ENTRIES; pte++) {
            PTE[pte] = (startRange + (pte * PAGE_SIZE)) | PTE_P | PTE_R | PTE_U;
        }

        startRange += PTE_TOTAL_ENTRIES * PAGE_SIZE;

        this->PDE[pde] = (uint32_t)PTE | PDE_P | PDE_R | PDE_U;

        PTE += PTE_TOTAL_ENTRIES;
    }

    __asm__ __volatile__(
        "movl %0, %%cr3;"
        "movl %%cr0, %%eax;"
        "orl $0x80000000, %%eax;"
        "movl %%eax, %%cr0;"
        :
        : "r" (this->PDE)
    );
}

void PgMgr::mapPage(void *vaddr, void *paddr) {
    if (((uint32_t)vaddr % PAGE_SIZE) || ((uint32_t)paddr % PAGE_SIZE)) {
        return;
    }

    uint32_t pde = this->PDE[(uint32_t)vaddr / (PTE_TOTAL_ENTRIES * PAGE_SIZE)];

    uint32_t* pte = (uint32_t*)(pde & 0xfffff000);

    pte[(uint32_t)vaddr % (PTE_TOTAL_ENTRIES * PAGE_SIZE) / PAGE_SIZE] =
                                        (uint32_t)paddr | PTE_P | PTE_R | PTE_U;
}

