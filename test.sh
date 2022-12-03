#!/bin/bash
assert() {
    expected="$1"
    input="$2"
    other="$3"

    ./kcc "$input" > tmp.s
    if [ "$other" = "" ]; then
        cc -o tmp tmp.s
    else
        cc -c ./test/"$other".c
        cc -o tmp tmp.s "$other".o
    fi
    ./tmp
    actual="$?"
    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

assert 0 "int main(){0;}"
assert 42 "int main(){42;}"
assert 21 "int main(){5+20-4;}"
assert 41 "int main(){ 12 + 34 - 5;}"
assert 47 "int main(){5+6*7;}"
assert 15 "int main(){5*(9-6);}"
assert 4 "int main(){(3+5)/2;}"
assert 10 "int main(){-10+20;}"
assert 2 "int main(){int a; a = 2;}"
assert 3 "int main(){int a; a=2+1;}"
assert 4 "int main(){int foo; foo = 3 + 1;}"
assert 5 "int main(){return 3+2;}"
assert 3 "int main(){int a; a=1;return a+2;}"
assert 3 "int main(){int A3; A3 = 3; return A3;}"
assert 1 "int main(){if(1 == 1) return 1;}"
assert 0 "int main(){if (1 == 2) return 1; else return 0;}"
assert 4 "int main(){int a; a = 2; int b; b = 2; while (a == b) a = a + 2; return a;}"
assert 2 "int main(){int a; a = 1; while (a == 1) a = a + 1; return a;}"
assert 3 "int main(){int a; for(a=0; a<3; a=a+1) 1; return a;}"
assert 14 "int main(){int a; int i; a = 0; i = 0; for(a=0; a<3; a=a+1) {i = i + 1; i = i * 2;} return i;}"
assert 2 "int main(){int i; i = 0; if(i == 0) { i = 1; i = i * 2;} else {i = 3; i = i + 1;} return i;}"
assert 0 "int main(){int i;{i = 1; i = i + 1; i = i - 2;} return i;}"
assert 5 "int main(){int i; i=foo(); return i;}" callfunc
assert 2 "int main(){int i; i=0; i=foo2(2); return i;}" callfunc
assert 5 "int main(){int i;i=0; i=foo3(2, 3); return i;}" callfunc
assert 21 "int main(){int i; i=0; i=foo4(1, 2, 3, 4, 5, 6); return i;}" callfunc
assert 1 "int main(){int i; i=0; foo5(); i=1; return i;}" callfunc
assert 3 "int main(){int a; int b; a=1;b = 1; return func()+b;} int func(){b = 2; return b;}"
assert 3 "int main(){return func(1, 2);} int func(int i, int j){return i+j;}"
assert 55 "int main(){return fib(10);} int fib(int n){if(n == 0){return 0;} else if(n == 1) {return 1;} else {return fib(n-1)+ fib(n-2);}}"
assert 3 "int main(){int x; int y; int z; x = 3; y = 5; z = &y + 8; return *z;}"
echo OK