#include <stdio.hpp>
#include <stdlib.hpp>
#include <syscall.hpp>

__thread int a[651] = {10}, b[651];
int c[541] = {20}, d[321];

int main(int argc, char **argv) {
    char str[10] = "hi";
    c[0]++;
    d[0]++;
    a[0]++;
    puts(str);

    return 0;
}

