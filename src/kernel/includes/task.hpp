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
            size_t tlsSize;
            CtxRegisters ctxReg;
            void *stack;
            uint8_t tid;
            bool statusEnd;
        }__attribute__((packed)) *TCB;

        struct ProcessHdr {
            int8_t pid;
            int8_t ppid;
            char filename[15];
            MMU::pgTbl *pd;
            void *heap;
            bool statusEnd;
        }__attribute__((packed)) *PCB;

        uint32_t size;
        size_t pgCount;
    }__attribute__((packed));

    class TaskMgr {
        private:
            TaskHeader *tasks[MAX_TASKS_COUNT];
            uint8_t tasksCount;
            uint8_t pids;
            uint64_t reapedTasksCount;

            Queue<uint8_t> tasksToReap;
            Queue<uint8_t> threadsQue[MAX_CPU_COUNT];

        public:
            uint8_t getTasksCount() const;
            uint64_t getReapedTasksCount() const;
            TaskHeader* schedule();
            void loadTask(char *fileName);
            void endTask(char *fileName);
            void endTask(int8_t pid);
            void endTask(uint8_t tid);
    };
}

extern TASK::TaskMgr taskMgr;

#endif  /*  TASK_HPP */
