#include <stdio.hpp>
#include <stdlib.hpp>
#include <syscall.hpp>

__thread int a[651] = {10}, b[651];
int c[541] = {20}, d[321];

static void hello1(int argc, char **argv) {

    for (int i = 0; i < 1; i++) {
        puts(argv[0]);
        puts(argv[1]);
    }
}

static void hello2(int argc, char **argv) {
    for (int i = 0; i < 2; i++) {
        puts(argv[0]);
        puts(argv[1]);
    }
}

static void hello3(int argc, char **argv) {
    for (int i = 0; i < 4; i++) {
        puts(argv[0]);
        puts(argv[1]);
    }
}

int main(int argc, char **argv) {
//    char str[30] = "0000000 @@@@@\n";
    char str1[30] = "1111111 #####\n";
    char str2[30] = "222222 $$$$$\n";
    char str3[30] = "33333333 &&&&&\n";
    c[0]++;
    d[0]++;
    a[0]++;
    uint8_t tid1 = createThread(hello1, str1);
    uint8_t tid2 = createThread(hello2, str2);
    uint8_t tid3 = createThread(hello3, str3);

    threadJoin(tid1);
    threadJoin(tid2);
    threadJoin(tid3);

    return 0;
}

