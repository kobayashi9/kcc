#include<stdio.h>

int foo() {
    return 5;
}

int foo2(int x) {
    return x;
}

int foo3(int x, int y) {
    return x + y;
}

int foo4(int x1, int x2, int x3, int x4, int x5, int x6) {
    return x1 + x2 + x3 + x4 + x5 + x6;
}

void foo5() {
    printf("hello world\n");
}