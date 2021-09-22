#include <stddef.h>
#include <stdint.h>

#ifndef SYSCALL_HPP
#define SYSCALL_HPP

#define SYS_PUTCH 0
#define SYS_PUTS 1
#define SYS_GETPID 2
#define SYS_GETPPID 3
#define SYS_GETTID 4
#define SYS_FORK 5
#define SYS_EXECVE 6
#define SYS_EXIT 7
#define SYS_MMAP 8
#define SYS_SCHED_YIELD 9
#define SYS_SLEEP 10
#define SYS_KILL 11
#define SYS_CREATE_THREAD 12
#define SYS_THREAD_JOIN 13
#define SYS_MUNMAP 14

extern "C" void putch(const char c);

extern "C" size_t puts(const char *str);

extern "C" uint8_t getpid();

extern "C" uint8_t getppid();

extern "C" uint8_t gettid();

extern "C" uint8_t fork();

extern "C" uint8_t execve(char *pathname, char **argv);

extern "C" void exit();

extern "C" void* mmap(void *addr, size_t length, int prot);

extern "C" void schedYield();

extern "C" void sleep(size_t ms);

extern "C" int createThread(void* (*func)(), size_t argc, char **argv);

extern "C" int threadJoin(uint8_t tid);

extern "C" int munmap(void *addr, size_t length);

#endif  /*  SYSCALL_HPP  */
