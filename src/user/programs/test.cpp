#include <stdio.hpp>
#include <stdlib.hpp>
#include <syscall.hpp>

static __thread uint8_t tid;
static __thread int8_t pid, ppid;

static void test1(int argc, char **argv) {
    tid = gettid();
    pid = getpid();
    ppid = getppid();

    printf("Hello from Thread %x, of Process %x, child of Process %x\n",
            tid, pid, ppid);
}

int main(int argc, char **argv) {

    uint8_t tid1 = createThread(test1, NULL);
    printf("Thread %x finished\n", tid1);
    threadJoin(tid1);
    printf("Thread %x finished\n", tid1);

    return 0;
}

