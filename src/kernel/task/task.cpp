#include <task.hpp>
#include <kstdlib.hpp>
#include <kstdio.hpp>
#include <kpkheap.hpp>
#include <kstring.hpp>
#include <gdt.hpp>

using namespace TASK;

TASK::TaskMgr taskMgr;

static inline void* alignPAGE_SIZE(void *addr) {
    uintptr_t _addr = (uintptr_t)addr;
    uintptr_t remainder = _addr % PAGE_SIZE;
    return (void*)(remainder ? (uint8_t*)addr + (PAGE_SIZE - remainder) : addr);
}

void TASK::TaskMgr::loadTask(char *fileName) {
    if (this->tasksCount == MAX_TASKS_COUNT) {
        kpwarn("Maximum tasks reached! Retry again later!\n");

        return;
    }

    this->tasksCount++;

    const char mode[] = "r";

    int fd = kfopen(fileName, mode);
    CATCH_FIRE(fd < 0, "Could not open file!");

    FILESYSTEM::FileStat *fst = kfstat(fd);
    CATCH_FIRE(fst == NULL, "Could not get file stats!");

    TaskHeader *task = (TaskHeader*)KPKHEAP::kpkZalloc(sizeof(*task));
    CATCH_FIRE(task == NULL, "Could not allocate task!");

    task->TCB = (TaskHeader::ThreadHdr*)KPKHEAP::kpkZalloc(sizeof(*task->TCB));
    CATCH_FIRE(task->TCB == NULL, "Could not allocate task TCB!");

    task->PCB = (TaskHeader::ProcessHdr*)KPKHEAP::kpkZalloc(sizeof(*task->PCB));
    CATCH_FIRE(task->PCB == NULL, "Could not allocate task PCB!");

    task->size = (uint32_t)((uintptr_t)alignPAGE_SIZE((void*)((uintptr_t)fst->size))) +
                                (TASK_USABLE_PAGES_COUNT * PAGE_SIZE);

    void *lastPg;
    for (size_t addr = USERSPACE_START_ADDR;
            addr < (size_t)(USERSPACE_START_ADDR + task->size);
            addr += PAGE_SIZE, task->pgCount++) {
        lastPg = pageManager.reqPg();
        pageManager.mapPg((void*)addr, lastPg, PDE_P | PDE_R | PDE_U);
    }

    task->PCB->pd = (MMU::pgTbl*)MMU::userPD->entries[0];

    lastPg = (void*)(USERSPACE_START_ADDR + task->size);

    task->TCB->stack = lastPg;

    task->TCB->tid = this->tasksCount++;

    task->TCB->statusEnd = false;

    task->PCB->heap = (uint8_t*)lastPg - TASK_STACK_SIZE - TASK_HEAP_SIZE;

    task->PCB->ppid = 0;

    task->PCB->pid = ++pids;

    task->PCB->statusEnd = false;

    task->TCB->ctxReg.rip = USERSPACE_START_ADDR;
    task->TCB->ctxReg.ss = USER_DATA;
    task->TCB->ctxReg.cs = USER_CODE;
    task->TCB->ctxReg.rsp = task->TCB->ctxReg.rbp = (uint64_t)task->TCB->stack;
    task->TCB->ctxReg.fs = (uint64_t)task->TCB->tlsp;

    kfread(fd, (void*)USERSPACE_START_ADDR, fst->size);

    memcpy(task->PCB->filename, fileName, sizeof(task->PCB->filename));

    memcpy((uint8_t*)task->PCB->heap - PAGE_SIZE, task, sizeof(*task));

    MMU::userPD->entries[0] = NULL;

    this->bspQue.push(task->TCB->tid);
}
