#include <stdio.hpp>
#include <stdlib.hpp>
#include <syscall.hpp>

static __thread uint8_t tid;
static __thread int8_t pid, ppid;

static void test(int argc, char **argv) {
    tid = gettid();
    pid = getpid();
    ppid = getppid();

    printf("Hello from TID %x, of PID %x, child of PPID %x\n!",
            tid, pid, ppid);
}

int main(int argc, char **argv) {
    tid = gettid();
    pid = getpid();
    ppid = getppid();

    printf("Hello from TID %x, of PID %x, child of PPID %x\n!",
            tid, pid, ppid);

    uint8_t childTid = createThread(test, NULL);
    threadJoin(childTid);
    printf("Thread %x finished\n", childTid);

    return 0;
}

