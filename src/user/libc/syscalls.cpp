#include <syscall.hpp>
#include <stdarg.h>
#include <stdio.hpp>

extern "C" void putch(const char c) {
    __asm__ __volatile__(
        "int $0x80;"
        :
        : "a" (SYS_PUTCH), "D" (c)
    );
}

extern "C" size_t puts(const char *str) {
    size_t ret;

    __asm__ __volatile__(
        "int $0x80;"
        : "=a"(ret)
        : "a" (SYS_PUTS), "D" (str)
    );

    return ret;
}

extern "C" uint8_t getpid() {
    uint8_t ret;

    __asm__ __volatile__(
        "int $0x80;"
        : "=a"(ret)
        : "a" (SYS_GETPID)
    );

    return ret;
}

extern "C" uint8_t getppid() {
    uint8_t ret;

    __asm__ __volatile__(
        "int $0x80;"
        : "=a"(ret)
        : "a" (SYS_GETPPID)
    );

    return ret;
}

extern "C" uint8_t gettid() {
    uint8_t ret;

    __asm__ __volatile__(
        "int $0x80;"
        : "=a"(ret)
        : "a" (SYS_GETTID)
    );

    return ret;
}

extern "C" uint8_t fork() {
    uint8_t ret;

    __asm__ __volatile__(
        "int $0x80;"
        : "=a"(ret)
        : "a" (SYS_FORK)
    );

    return ret;
}

extern "C" uint8_t execve(char *pathname, char **argv) {
    uint8_t ret;

    __asm__ __volatile__(
        "int $0x80;"
        : "=a"(ret)
        : "a" (SYS_EXECVE), "D" (pathname), "S" (argv)
    );

    return ret;
}

extern "C" void* mmap(void *addr, size_t length, int prot) {
    void *ret;

    __asm__ __volatile__(
        "int $0x80;"
        : "=a"(ret)
        : "a" (SYS_MMAP), "D" (addr), "S" (length), "d" (prot)
    );

    return ret;
}

extern "C" void schedYield() {
    __asm__ __volatile__(
        "int $0x80;"
        :
        : "a" (SYS_SCHED_YIELD)
    );
}

extern "C" void sleep(size_t ms) {
    __asm__ __volatile__(
        "int $0x80;"
        :
        : "a" (SYS_SLEEP), "D" (ms)
    );
}

extern "C" int createThread(void* (*func)(), size_t argc, char **argv) {
    int ret;

    __asm__ __volatile__(
        "int $0x80;"
        : "=a"(ret)
        : "a" (SYS_CREATE_THREAD), "D" (func), "S" (argc), "d" (argv)
    );

    return ret;
}

extern "C" int threadJoin(uint8_t tid) {
    int ret;

    __asm__ __volatile__(
        "int $0x80;"
        : "=a"(ret)
        : "a" (SYS_THREAD_JOIN), "D" (tid)
    );

    return ret;
}

extern "C" int munmap(void *addr, size_t length) {
    int ret;

    __asm__ __volatile__(
        "int $0x80;"
        : "=a"(ret)
        : "a" (SYS_MUNMAP), "D" (addr), "S" (length)
    );

    return ret;
}

