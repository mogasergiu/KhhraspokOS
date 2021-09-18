#include <syscall.hpp>
#include <stdarg.h>
#include <stdio.hpp>

/*
 * I know `putch`, `puts`, `sleep` and others are not real syscalls but I don't
 * see any easier way of getting text to the screen from userspace and I don't
 * need `timespec` structs for now
 */
long kpkSyscall(long sysNo, ...) {
    va_list ap;
    va_start(ap, sysNo);

    long ret;

    switch (sysNo) {

        // SYS_PUTCH
        case SYS_PUTCH:
            __asm__ __volatile__(
                "int $0x80;"
                : "=a"(ret)
                : "a" (sysNo), "D" (va_arg(ap, int))
            );

            break;

        case SYS_PUTS:
            __asm__ __volatile__(
                "int $0x80;"
                : "=a"(ret)
                : "a" (sysNo), "D" (va_arg(ap, char*))
            );

            break;

        case SYS_GETPID:
            __asm__ __volatile__(
                "int $0x80;"
                : "=a"(ret)
                : "a" (sysNo)
            );

            break;

        case SYS_GETPPID:
            __asm__ __volatile__(
                "int $0x80;"
                : "=a"(ret)
                : "a" (sysNo)
            );

            break;

        case SYS_GETTID:
            __asm__ __volatile__(
                "int $0x80;"
                : "=a"(ret)
                : "a" (sysNo)
            );

            break;

        case SYS_FORK:
            __asm__ __volatile__(
                "int $0x80;"
                : "=a"(ret)
                : "a" (sysNo)
            );

            break;

        case SYS_EXECVE:
            __asm__ __volatile__(
                "int $0x80;"
                : "=a"(ret)
                : "a" (sysNo), "D" (va_arg(ap, char*)),
                    "S" (va_arg(ap, char**))
            );

            break;

        case SYS_EXIT:
            __asm__ __volatile__(
                "int $0x80;"
                : "=a"(ret)
                : "a" (sysNo), "D" (va_arg(ap, char*))
            );

            break;

        case SYS_MMAP:
            __asm__ __volatile__(
                "int $0x80;"
                : "=a"(ret)
                : "a" (sysNo), "D" (va_arg(ap, void*)),
                "S" (va_arg(ap, size_t)), "d" (va_arg(ap, int))
            );

            break;

        case SYS_SCHED_YIELD:
            __asm__ __volatile__(
                "int $0x80;"
                : "=a"(ret)
                : "a" (sysNo)
            );

            break;

        case SYS_SLEEP:
            __asm__ __volatile__(
                "int $0x80;"
                : "=a"(ret)
                : "a" (sysNo), "D" (va_arg(ap, uint64_t))
            );

            break;

        case SYS_KILL:
            __asm__ __volatile__(
                "int $0x80;"
                : "=a"(ret)
                : "a" (sysNo), "D" (va_arg(ap, uintptr_t))
            );

            break;

        case SYS_CREATE_THREAD:
            __asm__ __volatile__(
                "int $0x80;"
                : "=a"(ret)
                : "a" (sysNo), "D" (va_arg(ap, void* (*)())),
                    "S" (va_arg(ap, size_t)), "d" (va_arg(ap, char**))
            );

            break;

        case SYS_THREAD_JOIN:
            __asm__ __volatile__(
                "int $0x80;"
                : "=a"(ret)
                : "a" (sysNo), "D" (va_arg(ap, uintptr_t))
            );

            break;

        case SYS_MUNMAP:
            __asm__ __volatile__(
                "int $0x80;"
                : "=a"(ret)
                : "a" (sysNo), "D" (va_arg(ap, void*)),
                "S" (va_arg(ap, size_t))
            );

            break;

//        default:
//            printf("%x Syscall does not exit yet!\n");
    }

    va_end(ap);

    return ret;
}

extern "C" void putch(const char c) {
    kpkSyscall(SYS_PUTCH, (int)c);
}

extern "C" size_t puts(const char *str) {
    return kpkSyscall(SYS_PUTS, str);
}

extern "C" uint8_t getpid() {
    return kpkSyscall(SYS_GETPID);
}

extern "C" uint8_t getppid() {
    return kpkSyscall(SYS_GETPPID);
}

extern "C" uint8_t gettid() {
    return kpkSyscall(SYS_GETTID);
}

extern "C" uint8_t fork() {
    return kpkSyscall(SYS_FORK);
}

extern "C" uint8_t execve(char *pathname, char **argv) {
    return kpkSyscall(SYS_EXECVE, pathname, argv);
}

extern "C" void* mmap(void *addr, size_t length, int prot) {
    return (void*)kpkSyscall(SYS_MMAP, addr, length, prot);
}

extern "C" void schedYield() {
    kpkSyscall(SYS_SCHED_YIELD);
}

extern "C" void sleep(size_t ms) {
    kpkSyscall(SYS_SLEEP, ms);
}

extern "C" int createThread(void* (*func)(), size_t argc, char **argv) {
    return kpkSyscall(SYS_CREATE_THREAD, func, argc, argv);
}

extern "C" int threadJoin(uint8_t tid) {
    return kpkSyscall(SYS_THREAD_JOIN, tid);
}

extern "C" int munmap(void *addr, size_t length) {
    return kpkSyscall(SYS_MUNMAP, addr, length);
}

