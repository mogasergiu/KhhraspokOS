#include <stdio.hpp>
#include <string.hpp>
#include <stdlib.hpp>
#include <syscall.hpp>

#define MAX_CMD_LENGTH 100
#define CMD_COUNT 5

char cmd[][MAX_CMD_LENGTH] = {
    "lspci",
    "free",
    "clear",
    "ps",
    "shutdown",
    };

char invalidCmd[] = "Invalid command!\n";

static __thread uint8_t tid;
static __thread int8_t pid, ppid;

int main(int argc, char **argv) {
    char buffer[MAX_CMD_LENGTH];
    int i;

    tid = gettid();
    pid = getpid();
    ppid = getppid();

    printf("Shell %x (TID), %x (PID), %x (PPID)\n", tid, pid, ppid);

    while (1) {
        prompt();

        fgets(buffer, sizeof(buffer));

        for (i = 0; i < CMD_COUNT; i++) {
            if (!strcmp(cmd[i], buffer)) {
                switch (i) {
                    case 0:
                        lspci();

                        break;

                    case 1:
                        free();

                        break;

                    case 2:
                        clear();

                        break;

                    case 3:
                        ps();

                        break;

                    case 4:
                        shutdown();

                        break;
                }

                break;
            }
        }

        if (i == CMD_COUNT) {
                puts(invalidCmd);
        }
    }

    return 0;
}

