#include <task.hpp>
#include <kstdlib.hpp>
#include <kstdio.hpp>
#include <kpkheap.hpp>
#include <kstring.hpp>
#include <gdt.hpp>
#include <elf.hpp>

using namespace TASK;

TASK::TaskMgr taskMgr;

TaskHeader* TASK::TaskMgr::schedule() {
    uint8_t id = apicHandler.getLAPICID();
 
    CtxRegisters *ctx;
    TaskHeader *oldTask;
    uintptr_t edx, eax;
     __asm__ __volatile__(
        "rdmsr;"
        "mov %%rbp, %0;"
        : "=r"(ctx), "=a" (eax), "=d" (edx)
        : "c" (0xc0000100)
    );

    oldTask = (TaskHeader*)((edx <<  32) + eax);

    if (oldTask != NULL) {
    if (oldTask->PCB->statusEnd || oldTask->TCB->statusEnd) {
        this->tasksToReap.push(oldTask->TCB->tid);

    } else {
        uint64_t fs = oldTask->TCB->ctxReg.fs;
        memcpy(&oldTask->TCB->ctxReg, ctx, sizeof(*ctx));
        oldTask->TCB->ctxReg.fs = fs;

        this->threadsQue[id].push(oldTask->TCB->tid);
    }}
 
    if (this->threadsQue[id].isEmpty()) {
        return NULL;
    }

    TaskHeader *task = this->tasks[*this->threadsQue[id].pop()];
    while (task != NULL && (task->PCB->statusEnd || task->TCB->statusEnd)) {
        this->tasksToReap.push(task->TCB->tid);

        task = this->tasks[*this->threadsQue[id].pop()];
    }
 
    MMU::userPD->entries[PDidx((void*)USERSPACE_START_ADDR)] = (uint64_t*)task->PCB->pd;
    __asm__ __volatile__(
        "mov %%cr3, %%r15;"
        "mov %%r15, %%cr3;"
        :
        :
    );

    return task;
}


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

    task->PCB->pd = (MMU::pgTbl*)MMU::userPD->entries[PDidx((void*)USERSPACE_START_ADDR)];

    lastPg = (void*)((uintptr_t)USERSPACE_START_ADDR + task->size);

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
    task->TCB->ctxReg.fs = (uint64_t)task;

    kfread(fd, (void*)((uintptr_t)USERSPACE_START_ADDR), fst->size);

    ELF *elf = (ELF*)((uintptr_t)USERSPACE_START_ADDR);
    elf++;
    elf--;

    memcpy(task->PCB->filename, fileName, sizeof(task->PCB->filename));

    memcpy((uint8_t*)task->PCB->heap - PAGE_SIZE, task, sizeof(*task));

    MMU::userPD->entries[PDidx((void*)USERSPACE_START_ADDR)] = NULL;

    __asm__ __volatile__(
        "mov %%cr3, %%r15;"
        "mov %%r15, %%cr3;"
        :
        :
    );

    this->threadsQue[1].push(task->TCB->tid);

    this->tasks[task->TCB->tid] = task;
}

uint8_t TaskMgr::getTasksCount() const {
    return this->tasksCount;
}

