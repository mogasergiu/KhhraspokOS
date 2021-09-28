#include <stdio.hpp>
#include <string.hpp>
#include <stdlib.hpp>
#include <syscall.hpp>

#define MAX_CMD_LENGTH 100
#define CMD_COUNT 8

char cmd[][MAX_CMD_LENGTH] = {
    "lspci",
    "free",
    "clear",
    "ps",
    "shutdown",
    "gettid",
    "getpid",
    "getppid"
    };

char invalidCmd[] = "Invalid command!\n";

static __thread uint8_t tid;
static __thread int8_t pid, ppid;

int main(int argc, char **argv) {
    char buffer[MAX_CMD_LENGTH];
    char binPath[6] = "/bin/";
    char ELFPath[20];
    int i;

    tid = gettid();
    pid = getpid();
    ppid = getppid();

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

                    case 5:
                        printf("Shell TID: %x\n", tid);

                        break;

                    case 6:
                        printf("Shell PID: %x\n", pid);

                        break;

                    case 7:
                        printf("Shell PPID: %x\n", ppid);

                        break;
                }

                break;
            }
        }

        if (i == CMD_COUNT) {
            memcpy(ELFPath, binPath, sizeof(binPath));
            strcat(ELFPath + sizeof(binPath) - 1, buffer);
            puts(ELFPath);
            int8_t childPid = createProcess(ELFPath);
            if (childPid < 0) {
                puts(invalidCmd);

            } else {
                threadJoin(childPid);
            }

            memset(ELFPath, 0, sizeof(ELFPath));
        }
    }

    return 0;
}

