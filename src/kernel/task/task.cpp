#include <task.hpp>
#include <kstdlib.hpp>
#include <kstdio.hpp>
#include <kpkheap.hpp>
#include <kstring.hpp>
#include <gdt.hpp>
#include <elf.hpp>
#include <video.hpp>

using namespace TASK;

TASK::TaskMgr taskMgr;

static uint8_t threadEnd[] = {0x48, 0xc7, 0xc0, 0x07, 0x0,
                            0x0, 0x0, 0xcd, 0x80, 0xeb, 0xfe};

TaskMgr::TaskMgr() {
    char kernelFilename[] = "Kernel Thread";
    this->kernelPHdr = (TaskHeader::ProcessHdr*)
                        KPKHEAP::kpkZalloc(sizeof(*this->kernelPHdr));
    CATCH_FIRE(this->kernelPHdr == NULL,
                "Could not allocate Kernel Task Header!\n");

    this->kernelPHdr->pid = -1;
    this->kernelPHdr->ppid = -1;
    this->kernelPHdr->lastVaddr = (void*)USERSPACE_START_ADDR;
    this->kernelPHdr->pd = NULL;
    this->kernelPHdr->heap = NULL;
    this->kernelPHdr->statusEnd = false;
    memcpy(this->kernelPHdr->filename, kernelFilename, sizeof(kernelFilename));
}

TaskHeader::ProcessHdr* TaskMgr::getKernelPHdr() const {
    return this->kernelPHdr;
}

void reaper(int argc, char **argv) {
    uint8_t *deadTid;

    while (1) {
        if (!taskMgr.tasksToReap.isEmpty()) {
            deadTid = taskMgr.tasksToReap.pop();
            taskMgr.freeTask(*deadTid);
        }
    }
}

void loader(int argc, char **argv) {
    uint8_t *newTid, ap = 1;
    TaskHeader *task;

    while (1) {
        if (!taskMgr.tasksToLoad.isEmpty()) {
            newTid = taskMgr.tasksToLoad.pop();

            task = taskMgr.tasks[*newTid];

            if (task != NULL) {
                if (task->TCB->ctxReg.rip != (uint64_t)&loader &&
                    task->TCB->ctxReg.rip != (uint64_t)&reaper) {
                    taskMgr.threadsQue[ap].push(*newTid);

                    ap++;
                    if (ap == *apicHandler.activeCPUs) {
                        ap = 1;
                    }

                } else {
                    taskMgr.threadsQue[0].push(*newTid);
                }
            }
        }
    }
}

void TaskMgr::freeTask(uint8_t tid) {
    TaskHeader *task = this->tasks[tid];

    if (task != NULL) {
        if (task->TCB != NULL && task->TCB->statusEnd == true) {
            KPKHEAP::kpkFree(task->TCB);
            task->TCB = NULL;
        }

        if (task->PCB != NULL) {
            if (task->PCB->pid == tid) {
                if (task->PCB->ogTLS != NULL) {
                    KPKHEAP::kpkFree(task->PCB->ogTLS);

                }

                MMU::pgTbl* pt = (MMU::pgTbl*)getPgAddr(task->PCB->pd);

                task->PCB->lastVaddr = (uint8_t*)task->PCB->lastVaddr - PAGE_SIZE;
                for (int i =
                    (int)((uintptr_t)
                    ((uint8_t*)task->PCB->lastVaddr - USERSPACE_START_ADDR)
                            / PAGE_SIZE);
                    i >= 0;
                    i--) {
                        pageManager.freePg(pt->entries[i]);
                }

                pageManager.freePg(pt);
            }

            KPKHEAP::kpkFree(task->PCB);
            task->PCB = NULL;
        }

        KPKHEAP::kpkFree(task);

        this->tasks[tid] = NULL;

        this->tasksCount--;
    }
}

TaskHeader* TASK::TaskMgr::schedule() {
    uint8_t id = apicHandler.getLAPICID();

    CtxRegisters *ctx;
    TaskHeader *oldTask;
    uintptr_t edx, eax;
    __asm__ __volatile__(
        "rdmsr;"
        "mov %%r15, %0;"
        : "=r"(ctx), "=a" (eax), "=d" (edx)
        : "c" (0xc0000101)
    );

    oldTask = (TaskHeader*)((edx <<  32) + eax);

    if (oldTask != NULL && oldTask->TCB != NULL && oldTask->PCB != NULL) {
        if (oldTask->PCB->statusEnd || oldTask->TCB->statusEnd) {
            this->tasksToReap.push(oldTask->TCB->tid);

        } else {
            if (oldTask->TCB->timeSlices > 0) {
                oldTask->TCB->timeSlices--;

                return NULL;
            }

            uint64_t gs = oldTask->TCB->ctxReg.gs;
            uint64_t fs = oldTask->TCB->ctxReg.fs;
            memcpy(&oldTask->TCB->ctxReg, ctx, sizeof(*ctx));
            oldTask->TCB->ctxReg.gs = gs;
            oldTask->TCB->ctxReg.fs = fs;

            this->threadsQue[id].push(oldTask->TCB->tid);
        }
    }
 
    while (this->threadsQue[id].isEmpty()) {
        __asm__ __volatile__(
            "pause"
            :
            :
        );
    }

    TaskHeader *task = this->tasks[*this->threadsQue[id].pop()];
    while (task != NULL && (task->PCB->statusEnd || task->TCB->statusEnd)) {
        this->tasksToReap.push(task->TCB->tid);

        while (this->threadsQue[id].isEmpty()) {
            __asm__ __volatile__(
                "pause"
                :
                :
            );
        }

        task = this->tasks[*this->threadsQue[id].pop()];
    }

    task->TCB->timeSlices = task->estimate * TIME_SLICE;

    MMU::userPD->entries[PDidx((void*)USERSPACE_START_ADDR)] = (uint64_t*)task->PCB->pd;
    flushCR3();

    return task;
}


static inline void* alignPAGE_SIZE(void *addr) {
    uintptr_t _addr = (uintptr_t)addr;
    uintptr_t remainder = _addr % PAGE_SIZE;
    return (void*)(remainder ? (uint8_t*)addr + (PAGE_SIZE - remainder) : addr);
}

int __attribute__((optimize("O3"))) TASK::TaskMgr::createTask(char *args, uint8_t dpl, int8_t ppid) {
    if (this->tasksCount == MAX_TASKS_COUNT) {
        kpwarn("Maximum tasks reached! Retry again later!\n");

        return -1;
    }

    const char mode[] = "r", filter[] = " ";
    char _args[100];
    memcpy(_args, args, sizeof(_args));

    char *arg = strtok(_args, filter);
    if (arg == NULL) {
        kpwarn("Incorrect executable/task name!\n");

        return -1;
    }

    int fd = kfopen(arg, mode);
    if (fd < 0) {
        kpwarn("No such ELF!\n");

        return -1;
    }

    FILESYSTEM::FileStat *fst = kfstat(fd);
    CATCH_FIRE(fst == NULL, "Could not get file stats!");

    TaskHeader *task = (TaskHeader*)KPKHEAP::kpkZalloc(sizeof(*task));
    CATCH_FIRE(task == NULL, "Could not allocate task!");

    task->TCB = (TaskHeader::ThreadHdr*)KPKHEAP::kpkZalloc(sizeof(*task->TCB));
    CATCH_FIRE(task->TCB == NULL, "Could not allocate task TCB!");

    task->PCB = (TaskHeader::ProcessHdr*)KPKHEAP::kpkZalloc(sizeof(*task->PCB));
    CATCH_FIRE(task->PCB == NULL, "Could not allocate task PCB!");

    void *buffer = KPKHEAP::kpkZalloc(fst->size + 65535);
    CATCH_FIRE(buffer == NULL, "Could not allocate Heap to read the ELF!\n");

    kfread(fd, buffer, fst->size);

    uint64_t *oldPD = MMU::userPD->entries[PDidx((void*)USERSPACE_START_ADDR)];
    MMU::userPD->entries[PDidx((void*)USERSPACE_START_ADDR)] = NULL;
    flushCR3();

    void *lastVaddr = loadELF(buffer, task);
    CATCH_FIRE(lastVaddr == NULL, "Could not load ELF!\n");

    KPKHEAP::kpkFree(buffer);
    kfclose(fd);

    void *lastPg = pageManager.reqPg();
    pageManager.mapPg(lastVaddr, lastPg, PDE_P | PDE_R | PDE_U);

    lastVaddr= (uint8_t*)lastVaddr + PAGE_SIZE;

    task->PCB->pd = (MMU::pgTbl*)MMU::userPD->entries[PDidx((void*)USERSPACE_START_ADDR)];

    task->TCB->stack = (uint8_t*)lastVaddr;

    task->TCB->tid = this->tasksCount++;

    task->TCB->statusEnd = false;

    task->PCB->ppid = ppid;

    task->PCB->pid = task->TCB->tid;

    task->PCB->statusEnd = false;

    if (dpl == 3) {
        task->TCB->ctxReg.ss = USER_DATA;
        task->TCB->ctxReg.cs = USER_CODE;

    } else {
        task->TCB->ctxReg.ss = KERNEL_DATA;
        task->TCB->ctxReg.cs = KERNEL_CODE;
    }

    task->TCB->ctxReg.rsp = task->TCB->ctxReg.rbp = (uint64_t)task->TCB->stack;
    task->TCB->ctxReg.gs = (uint64_t)task;

    memcpy(task->PCB->filename, arg, sizeof(task->PCB->filename));

//    this->threadsQue[0].push(task->TCB->tid);

    this->tasks[task->TCB->tid] = task;

    lastPg = pageManager.reqPg();
    pageManager.mapPg(lastVaddr, lastPg, PDE_P | PDE_R | PDE_U);

    task->PCB->lastVaddr = (uint8_t*)lastVaddr + PAGE_SIZE;
   
    arg = strtok(_args + strlen(_args) + 1, filter);
    size_t argSize = strlen(arg);
    int argc = 0;
    char *argv0 = (char*)lastVaddr;
    while (arg != NULL) {
        argc++;

        strncpy((char*)lastVaddr, arg, argSize);

        lastVaddr = (uint8_t*)lastVaddr + argSize + 1;

        arg = strtok(NULL, filter);
        argSize = strlen(arg);
    }

    char **argv = (char**)lastVaddr;
    for (int i = 0; i < argc; i++) {
        argv[i] = argv0;

        for (; *argv0 != 0; argv0++);
        argv0++;
    }

    task->TCB->env.argv = argv;
    task->TCB->env.argc = argc;
    task->TCB->ctxReg.rdi = argc;
    task->TCB->ctxReg.rsi = (uint64_t)argv;

    MMU::userPD->entries[PDidx((void*)USERSPACE_START_ADDR)] = oldPD;

    flushCR3();

    this->threadsQue[0].push(task->TCB->tid);

    return (int)task->TCB->tid;
}

uint8_t TASK::TaskMgr::createTask(void (*func)(int, char**), uint8_t dpl,
                                char *args, TASK::TaskHeader::ProcessHdr *PCB) {
    if (this->tasksCount == MAX_TASKS_COUNT) {
        kpwarn("Maximum tasks reached! Retry again later!\n");

        return 0;
    }

    MMU::userPD->entries[PDidx((void*)USERSPACE_START_ADDR)] = (uint64_t*)PCB->pd;
    flushCR3();

    const char filter[] = " ";

    void *lastPg;
    uint8_t pgCount = 0;

    TaskHeader *task = (TaskHeader*)KPKHEAP::kpkZalloc(sizeof(*task));
    CATCH_FIRE(task == NULL, "Could not allocate task!");

    task->TCB = (TaskHeader::ThreadHdr*)KPKHEAP::kpkZalloc(sizeof(*task->TCB));
    CATCH_FIRE(task->TCB == NULL, "Could not allocate task TCB!");

    uintptr_t flags;
    if (dpl == 3) {
        task->TCB->ctxReg.ss = USER_DATA;
        task->TCB->ctxReg.cs = USER_CODE;
        flags = PDE_P | PDE_R | PDE_U;

    } else {
        task->TCB->ctxReg.ss = KERNEL_DATA;
        task->TCB->ctxReg.cs = KERNEL_CODE;
        flags = PDE_P | PDE_R;
    }

    task->PCB = PCB;
    char *arg;
    int argc = 0;
    if (args != NULL) {
        arg = strtok(args, filter);
        lastPg = pageManager.reqPg();
        pgCount++;
        pageManager.mapPg(task->PCB->lastVaddr, lastPg, flags);

        char *argv0 = (char*)task->PCB->lastVaddr;
        size_t len;
        while (arg != NULL) {
            argc++;
            len = strlen(arg);
            memcpy(argv0, arg, len);
            argv0 += len + 1;
            arg = strtok(NULL, filter);
        }

        task->TCB->env.argc = argc;
        task->TCB->env.argv = (char**)argv0;
        argv0 = (char*)task->PCB->lastVaddr;
        for (int i = 0; i < argc; i++) {
            task->TCB->env.argv[i] = argv0;
            argv0 += strlen(argv0) + 1;
        }

        task->PCB->lastVaddr = (uint8_t*)task->PCB->lastVaddr + PAGE_SIZE;
    }


    lastPg = pageManager.reqPg();
    pgCount++;
    pageManager.mapPg(task->PCB->lastVaddr, lastPg, flags);

    task->PCB->lastVaddr = (uint8_t*)task->PCB->lastVaddr + PAGE_SIZE;

    task->TCB->stack = (uint8_t*)task->PCB->lastVaddr - 32;

    ((uintptr_t*)task->TCB->stack)[0] = (uintptr_t)task->TCB->stack + sizeof(uintptr_t);

    memcpy((uintptr_t*)task->TCB->stack + 1, threadEnd, sizeof(threadEnd));

    task->TCB->tid = this->tasksCount++;

    task->TCB->statusEnd = false;

    task->TCB->ctxReg.rsp = task->TCB->ctxReg.rbp = (uint64_t)task->TCB->stack;
    task->TCB->ctxReg.gs = (uint64_t)task;

    this->tasks[task->TCB->tid] = task;

    if (PCB->ogTLS != NULL && PCB->tlsSize > 0) {
        void *lastVaddr = task->PCB->lastVaddr;

        for (size_t i = 0; (i * PAGE_SIZE) < PCB->tlsSize; i++,
            task->PCB->lastVaddr = (uint8_t*)task->PCB->lastVaddr + PAGE_SIZE) {
            lastPg = pageManager.reqPg();
            pageManager.mapPg(task->PCB->lastVaddr, lastPg, flags);
        }

        memcpy(lastVaddr, PCB->ogTLS, PCB->tlsSize);
        task->TCB->ctxReg.fs = (uint64_t)lastVaddr + PCB->tlsSize;
        ((uint64_t*)task->TCB->ctxReg.fs)[0] = task->TCB->ctxReg.fs;
    }

    lastPg = pageManager.reqPg();
    pgCount++;
    pageManager.mapPg(task->PCB->lastVaddr, lastPg, flags);
    task->PCB->lastVaddr = (uint8_t*)task->PCB->lastVaddr + PAGE_SIZE;
   
    task->TCB->ctxReg.rdi = task->TCB->env.argc;
    task->TCB->ctxReg.rsi = (uint64_t)task->TCB->env.argv;
    task->TCB->ctxReg.rip = ((uint64_t)func);

    task->PCB = (TaskHeader::ProcessHdr*)KPKHEAP::kpkZalloc(sizeof(*task->PCB));
    CATCH_FIRE(task->PCB == NULL, "Could not allocate task PCB!");
    memcpy(task->PCB, PCB, sizeof(*task->PCB));

    task->estimate = pgCount << 2;

    task->PCB->pd = (MMU::pgTbl*)MMU::userPD->entries[PDidx((void*)USERSPACE_START_ADDR)];
    flushCR3();

    if (func == &loader) {
        this->threadsQue[0].push(task->TCB->tid);

    } else {
        this->tasksToLoad.push(task->TCB->tid);
    }

    return task->TCB->tid;
}

uint8_t TaskMgr::getTasksCount() const {
    return this->tasksCount;
}

bool TaskMgr::taskReady(uint8_t tid) const {
    return this->tasks[tid] == NULL;
}

extern "C" void TASK::schedYield() {
    uintptr_t eax, edx;
    TASK::TaskHeader *task;

    __asm__ __volatile__(
        "rdmsr;"
        : "=a" (eax), "=d" (edx)
        : "c" (0xc0000101)
    );

    task = (TASK::TaskHeader*)((edx <<  32) + eax);

    task->TCB->timeSlices = 0;  // yield
}

void TaskMgr::endTask(int8_t pid) {
    TaskHeader *task = NULL;

    volatile uint8_t initialTasksCount = this->tasksCount;
    for (uint8_t i = 0; i <= initialTasksCount; i++) {
        if (this->tasks[i] != NULL) {
            if (this->tasks[i]->PCB->pid == pid &&
                this->tasks[i]->TCB->tid != pid) {
                task = this->tasks[i];

                task->TCB->statusEnd = task->PCB->statusEnd = true;
                task->TCB->timeSlices = 0;

                while (this->tasks[i] != NULL);
            }
        }
    }
}

void TaskMgr::endTask() {
    TaskHeader *task;
    uintptr_t edx, eax;

    __asm__ __volatile__(
        "rdmsr;"
        : "=a" (eax), "=d" (edx)
        : "c" (0xc0000101)
    );

    task = (TaskHeader*)((edx <<  32) + eax);
 
    if (task->TCB->tid == task->PCB->pid) {
        this->endTask(task->PCB->pid);
    }

    task->TCB->statusEnd = task->PCB->statusEnd = true;
    task->TCB->timeSlices = 0;
}

void TaskMgr::printPS() const {
    vgaHandler.putString("\n{TID, PID, PPID, Process Name}\n", 15);
    TaskHeader *task;

    for (int i = 0; i < this->tasksCount; i++) {
        task = this->tasks[i];

        if (task == NULL) {
            continue;
        }

        kprintf("{%x, %x, %x, %s}\n",
                task->TCB->tid,
                task->PCB->pid,
                task->PCB->ppid,
                task->PCB->filename);
    }
}
