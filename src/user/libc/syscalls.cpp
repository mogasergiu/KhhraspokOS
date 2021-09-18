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
                    "S" (va_arg(ap, char*)), "d" (va_arg(ap, char*))
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
                : "a" (sysNo), "D" (va_arg(ap, int))
            );

            break;

        case SYS_CREATE_THREAD:
            __asm__ __volatile__(
                "int $0x80;"
                : "=a"(ret)
                : "a" (sysNo), "D" (va_arg(ap, int)),
                    "S" (va_arg(ap, char**))
            );

            break;

        case SYS_THREAD_JOIN:
            __asm__ __volatile__(
                "int $0x80;"
                : "=a"(ret)
                : "a" (sysNo), "D" (va_arg(ap, int))
            );

            break;

        case SYS_MUNMAP:
            __asm__ __volatile__(
                "int $0x80;"
                : "=a"(ret)
                : "a" (sysNo), "D" (va_arg(ap, void*)),
                "S" (va_arg(ap, size_t)), "d" (va_arg(ap, int))
            );

            break;

//        default:
//            printf("%x Syscall does not exit yet!\n");
    }

    va_end(ap);

    return ret;
}
