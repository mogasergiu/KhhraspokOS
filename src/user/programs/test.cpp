#include <stdio.hpp>
#include <stdlib.hpp>
#include <syscall.hpp>

static __thread uint8_t tid;
static __thread int8_t pid, ppid;

static void test(int argc, char **argv) {
    tid = gettid();
    pid = getpid();
    ppid = getppid();

    printf("Hello from Thread %x, of Process %x, child of Process %x\n!",
            tid, pid, ppid);

    printf("I received %d arguments!\n", argc);

    for (int i = 0; i < argc; i++) {
        printf("Argument %d: %s\n", i, argv[i]);
    }
}

int main(int argc, char **argv) {
    tid = gettid();
    pid = getpid();
    ppid = getppid();

    char someArgs[] = "arg-1 arg-2 arg-3";

    printf("Hello from Thread %x, of Process %x, child of Process %x\n!",
            tid, pid, ppid);
    printf("I received %d arguments!\n", argc);

    for (int i = 0; i < argc; i++) {
        printf("Argument %d: %s\n", i, argv[i]);
    }

    uint8_t childTid1 = createThread(test, NULL);
    threadJoin(childTid1);
    printf("Thread %x finished\n", childTid1);
    uint8_t childTid2 = createThread(test, someArgs);
    threadJoin(childTid2);
    printf("Thread %x finished\n", childTid2);

    return 0;
}

