#include <stddef.h>
#include <stdint.h>
#include <kdsa.hpp>
#include <memory.hpp>
#include <interrupts.hpp>

#ifndef TASK_HPP
#define TASK_HPP

#define MAX_TASKS_COUNT 0xff
#define TASK_USABLE_PAGES_COUNT 0xa
#define TASK_STACK_SIZE 0x2000
#define TASK_HEAP_SIZE 0x4000
#define TASK_TLS_SIZE 0x2000

namespace TASK {
    class TaskMgr;
};

void reaper(int argc, char **argv);
void loader(int argc, char **argv);

namespace TASK {
    extern "C" void acquireLock(volatile void *lock);
    extern "C" void releaseLock(volatile void *lock);
    struct CtxRegisters {
        uint64_t rax;
        uint64_t rbx;
        uint64_t rcx;
        uint64_t rdx;
        uint64_t rdi;
        uint64_t rsi;
        uint64_t rbp;
        uint64_t r8;
        uint64_t r9;
        uint64_t r10;
        uint64_t r11;
        uint64_t r12;
        uint64_t r13;
        uint64_t r14;
        uint64_t r15;

        uint64_t rip;
        uint64_t cs;
        uint64_t flags;
        uint64_t rsp;
        uint64_t ss;
        uint64_t gs;
        uint64_t fs;
    }__attribute__((packed));

    extern "C" void ret2User(CtxRegisters *ctxReg);

    struct TaskHeader {
        struct ThreadHdr {
            CtxRegisters ctxReg;
            void *stack;
            struct {
                int argc;
                char **argv;
            } env;
            uint8_t tid;
            bool statusEnd;
        }__attribute__((packed)) *TCB;

        struct ProcessHdr {
            int8_t pid;
            int8_t ppid;
            char filename[15];
            void *lastVaddr;
            MMU::pgTbl *pd;
            void *heap;
            bool statusEnd;
            size_t tlsSize;
            void *ogTLS;
            size_t threadPoolCount;
        }__attribute__((packed)) *PCB;

        uint32_t size;
    }__attribute__((packed));

    class TaskMgr {
        private:
            TaskHeader *tasks[MAX_TASKS_COUNT];
            uint8_t tasksCount;
            uint8_t pids;
            uint64_t reapedTasksCount;
            TaskHeader::ProcessHdr *kernelPHdr;

            Queue<uint8_t> tasksToReap;
            Queue<uint8_t> tasksToLoad;
            Queue<uint8_t> threadsQue[MAX_CPU_COUNT];

        public:
            TaskMgr();
            uint8_t getTasksCount() const;
            uint64_t getReapedTasksCount() const;
            TaskHeader::ProcessHdr* getKernelPHdr() const;
            TaskHeader* schedule();
            friend void ::reaper(int argc, char **argv);
            friend void ::loader(int argc, char **argv);
            friend void INTERRUPTS::IntCallbacks::lapicTimerIRQ();
            void createTask(char *args, uint8_t dpl, int8_t ppid);
            uint8_t createTask(void (*)(int, char**), uint8_t dpl,
                                char *args, TASK::TaskHeader::ProcessHdr *PCB);
            void loadTask(TaskHeader *task);
            void freeTask(uint8_t tid);
            void endTask(int8_t pid);
            void endTask();
            bool taskReady(uint8_t tid) const;
    };
}

extern TASK::TaskMgr taskMgr;

#endif  /*  TASK_HPP */
