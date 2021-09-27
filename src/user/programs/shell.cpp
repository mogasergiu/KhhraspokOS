#include <stdio.hpp>
#include <string.hpp>
#include <stdlib.hpp>
#include <syscall.hpp>

int main(int argc, char **argv) {

    prompt();
    lspci();
    free();
    ps();

    printf("%x hi %x\n", 1, 2);

    char buf[100];
    fgets(buf, sizeof(buf));
    printf("%s\n", buf);

    return 0;
}

