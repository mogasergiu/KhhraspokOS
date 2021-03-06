#include <memory.hpp>
#include <kpkheap.hpp>
#include <video.hpp>
#include <kstring.hpp>

using namespace MMU;

MMU::memRegionDescriptor* MMU::memMap = (memRegionDescriptor*)MEM_MAP_ADDR;
size_t MMU::usedMem = KERNEL_USED_MEM;
size_t MMU::reservedMem;
size_t MMU::freeMem;
uint8_t MMU::memRegionCount = *(uint8_t*)MEM_REGION_COUNT_ADDR;
MMU::PgFrAllocator MMU::PgMgr::pgAllocator;
MMU::PgMgr pageManager;
MMU::pgTbl* MMU::userPD;

void PgFrAllocator::initPgAlloc() {
    size_t memSize, pg;
    MMU::memRegionCount = *(uint8_t*)MEM_REGION_COUNT_ADDR;
    MMU::freeMem = MMU::memMap[MMU::memRegionCount - 1].baseAddr +
                       MMU::memMap[MMU::memRegionCount - 1].length;
    memSize = MMU::freeMem;

    this->PgsMap = (BitMap*)KPKHEAP::kpkZalloc(sizeof(*this->PgsMap));
    this->PgsMap->initBitMap((memSize  / PAGE_SIZE));

    for (MMU::memRegionDescriptor *md = MMU::memMap;
            md < (MMU::memMap + MMU::memRegionCount); md++) {
        if (md->type != USABLE_MEM) {
            for (pg = (md->baseAddr - (md->baseAddr % PAGE_SIZE));
                pg < ((md->baseAddr + md->length) +
                    (md->baseAddr + md->length) % PAGE_SIZE); pg += PAGE_SIZE) {
                this->PgsMap->set(pg / PAGE_SIZE, true);
            }
        }
    }

    // map the bootloader
    for (pg = 0; pg < 0x9000; pg += PAGE_SIZE) {
        this->allocPg((void*)pg);
    }

    this->bitmapIdx = pg / PAGE_SIZE;

    // map the video memory
    for (pg = 0x80000; pg < 0xfffff; pg += PAGE_SIZE) {
        this->allocPg((void*)pg);
    }

    // map the kernel
    for (pg = KERNEL_START_ADDR; pg < KERNEL_USED_MEM; pg += PAGE_SIZE) {
        this->allocPg((void*)pg);
    }

    // map kernel heap
    for (pg = KPKHEAP_START; pg < KPKHEAP_END; pg += PAGE_SIZE) {
        this->allocPg((void*)pg);
    } 
}

// returns physical page
void* PgFrAllocator::reqPg() {
    size_t len = this->PgsMap->getSize();

    for (; this->bitmapIdx < len * 8; this->bitmapIdx++) {
        if (this->PgsMap->get(this->bitmapIdx) == true) {
            continue;
        }

        this->allocPg((void*)(this->bitmapIdx * PAGE_SIZE));

        this->bitmapIdx++;

        return (void*)((this->bitmapIdx - 1) * PAGE_SIZE);
    }

    return NULL;
}

// frees physical page
void PgFrAllocator::freePg(void* addr) {
    size_t idx = (size_t)addr / PAGE_SIZE;

    if (this->PgsMap->get(idx) == false) {
        return;
    }

    if (this->PgsMap->set(idx, false)) {
        MMU::freeMem += PAGE_SIZE;
        MMU::usedMem -= PAGE_SIZE;

        if (this->bitmapIdx > idx) {
            this->bitmapIdx = idx;
        }
    }
}

void PgFrAllocator::freePgs(void* addr, size_t count) {
    for (size_t idx = 0; idx < count; idx++) {
        this->freePg((void*)((size_t)addr + (idx * PAGE_SIZE)));
    }
}

void PgFrAllocator::allocPg(void* addr) {
    size_t idx = (size_t)addr / PAGE_SIZE;

    if (this->PgsMap->get(idx) == true) {
        return;
    }

    if (this->PgsMap->set(idx, true)) {
        MMU::freeMem -= PAGE_SIZE;
        MMU::usedMem += PAGE_SIZE;
    }
}

void PgFrAllocator::allocPgs(void* addr, size_t count) {
    for (size_t idx = 0; idx < count; idx++) {
        this->allocPg((void*)((size_t)addr + (idx * PAGE_SIZE)));
    }
}

// used to find the inter-changeable directory for userspace processes
inline static void extractUserPD(void *vaddr) {
    size_t idx;
    MMU::userPD = getPgAddr(vaddr);
    idx = PDTidx((void*)USERSPACE_START_ADDR);
    MMU::userPD = getPgAddr(MMU::userPD->entries[idx]);
    MMU::userPD->entries[PDidx((void*)USERSPACE_START_ADDR)] = NULL;
}

PgMgr::PgMgr() {
    this->pgAllocator.initPgAlloc();

    size_t memSize = MMU::memMap[MMU::memRegionCount - 1].baseAddr +
                       MMU::memMap[MMU::memRegionCount - 1].length;
;

    this->PML4 = (PML4struct*)PML4T_ADDR;
 
    // map kernel memory and its heap
    for (size_t pg = KERNEL_USED_MEM; pg < memSize; pg += PAGE_SIZE) {
        this->mapPg((void*)pg, (void*)pg, PDE_P | PDE_R);
    }

    this->mapPg((void*)USERSPACE_START_ADDR, (void*)USERSPACE_START_ADDR,
                PDE_P | PDE_R | PDE_U);

    size_t idx = PML4idx((void*)USERSPACE_START_ADDR);
    this->PML4->entries[idx] = (MMU::pgTbl*)((uint64_t)this->PML4->entries[idx] | PDE_U);
    MMU::userPD = getPgAddr(this->PML4->entries[idx]);
    idx = PDTidx((void*)USERSPACE_START_ADDR);
    MMU::userPD->entries[idx] = (uint64_t*)((uint64_t)MMU::userPD->entries[idx] | PDE_U);
    MMU::userPD = getPgAddr(MMU::userPD->entries[idx]);
    MMU::userPD->entries[PDidx((void*)USERSPACE_START_ADDR)] = NULL;
}

void* PgMgr::reqPg() {
    return this->pgAllocator.reqPg();
}

void PgMgr::freePg(void *addr) {
    return this->pgAllocator.freePg(addr);
}

// do a table walk to map vaddr to paddr
void PgMgr::mapPg(void *vaddr, void *paddr, uintptr_t flags) {
    size_t idx = PML4idx(vaddr);
    pgTbl *pdt = NULL,
          *pd = NULL,
          *pt = NULL,
          *pte = NULL,
          *tmpPg = NULL;

    if (!getPgFlag(this->PML4->entries[idx], PDE_P)) {
        tmpPg = (pgTbl*)this->pgAllocator.reqPg();

        memset(tmpPg, 0, PAGE_SIZE);

        setPgAddr(pdt, tmpPg);
        setPgFlag(pdt, flags);

        this->PML4->entries[idx] = pdt;

    }

    pdt = getPgAddr(this->PML4->entries[idx]);

    idx = PDTidx(vaddr);
    if (!getPgFlag(pdt->entries[idx], PDE_P)) {
        tmpPg = (pgTbl*)this->pgAllocator.reqPg();

        memset(tmpPg, 0, PAGE_SIZE);

        setPgAddr(pd, tmpPg);
        setPgFlag(pd, flags);

        pdt->entries[idx] = (uint64_t*)pd;

    }

    pd = getPgAddr(pdt->entries[idx]);

    idx = PDidx(vaddr);
    if (!getPgFlag(pd->entries[idx], PDE_P)) {
        tmpPg = (pgTbl*)this->pgAllocator.reqPg();

        memset(tmpPg, 0, PAGE_SIZE);

        setPgAddr(pt, tmpPg);
        setPgFlag(pt, flags);

        pd->entries[idx] = (uint64_t*)pt;

    }

    pt = getPgAddr(pd->entries[idx]);

    idx = PTidx(vaddr);

    setPgAddr(pte, paddr);
    setPgFlag(pte, flags);

    pt->entries[idx] = 0;
    pt->entries[idx] = (uint64_t*)pte;
}

