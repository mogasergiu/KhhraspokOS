#include <memory.hpp>
#include <kpkheap.hpp>
#include <video.hpp>

using namespace MMU;

PgMgr::PgMgr() {
/*    this->PML4T = (uintptr_t*)PML4T_ADDR;

    for (int pml4tIdx = 0; pml4tIdx < 4; pml4tIdx++) {
        uintptr_t *PDPT = this->PML4T[pml4tIdx];*/
/*
    this->PDE = (uintptr_t*)PDE_ENTRIES_ADDR;

    int startRange = 0;
    uintptr_t *PTE = (uintptr_t*)PTE_ENTRIES_ADDR;

    for (int pde = 0; pde < PDE_TOTAL_ENTRIES; pde++) {

        for (int pte = 0; pte < PTE_TOTAL_ENTRIES; pte++) {
            PTE[pte] = (startRange + (pte * PAGE_SIZE)) | PTE_P | PTE_R | PTE_U;
        }

        startRange += PTE_TOTAL_ENTRIES * PAGE_SIZE;

        this->PDE[pde] = (uintptr_t)PTE | PDE_P | PDE_R | PDE_U;

        PTE += PTE_TOTAL_ENTRIES;
    }
*/
}

void PgMgr::mapPage(void *vaddr, void *paddr) {
/*    if (((uintptr_t)vaddr % PAGE_SIZE) || ((uintptr_t)paddr % PAGE_SIZE)){
        return;
    }

    uintptr_t pde = this->PDE[(uintptr_t)vaddr / (PTE_TOTAL_ENTRIES * PAGE_SIZE)];

    uintptr_t* pte = (uintptr_t*)(pde & 0xfffff000);

    pte[(uintptr_t)vaddr % (PTE_TOTAL_ENTRIES * PAGE_SIZE) / PAGE_SIZE] =
                                        (uintptr_t)paddr | PTE_P | PTE_R | PTE_U;*/
}

