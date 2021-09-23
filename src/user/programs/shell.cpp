#include <stdio.hpp>
#include <stdlib.hpp>
#include <syscall.hpp>

__thread int a[651] = {10}, b[651];
int c[541] = {20}, d[321];

static void hello1(int argc, char **argv) {
    char str[10] = "a b \n";

    while (1)
    puts(str);
}

static void hello2(int argc, char **argv) {
    char str[10] = "c d \n";

    while (1)
    puts(str);
}

static void hello3(int argc, char **argv) {
    char str[10] = "e f \n";
    while (1)
    puts(str);
}

int main(int argc, char **argv) {
    char str[10] = "lol\n";
    c[0]++;
    d[0]++;
    a[0]++;
    createThread(hello1, NULL);
    createThread(hello2, NULL);
    createThread(hello3, NULL);
    while (1)
    puts(str);
    return 0;
}

