#include <stdio.hpp>
#include <stdlib.hpp>
#include <syscall.hpp>

__thread int a[651] = {10}, b[651];
int c[541] = {20}, d[321];

char str[] = "Hi";

class Test {
    public:
        Test() {
            puts(str);
        }
};

Test test;

int main(char argc, char **argv) {
    c[0]++;
    d[0]++;
    a[0]++;
    puts(str);
    return 0;
}

