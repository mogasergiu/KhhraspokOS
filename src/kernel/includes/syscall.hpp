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
#define SYS_PROMPT 15
#define SYS_LSPCI 16
#define SYS_FREE 17
#define SYS_CLEAR 18
#define SYS_PS 19
#define SYS_SHUTDOWN 20
#define SYS_REBOOT 21
#define SYS_FGETS 22
#define SYS_CREATE_PROCESS 23

#endif  /*  SYSCALL_HPP  */
