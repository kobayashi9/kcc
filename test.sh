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

assert 0 "0;"
assert 42 "42;"
assert 21 "5+20-4;"
assert 41 " 12 + 34 - 5;"
assert 47 "5+6*7;"
assert 15 "5*(9-6);"
assert 4 "(3+5)/2;"
assert 10 "-10+20;"
assert 2 "a=2;"
assert 3 "a=2+1;"
assert 4 "foo = 3 + 1;"
assert 5 "return 3+2;"
assert 3 "a=1;return a+2;"
assert 3 "A3 = 3; return A3;"
assert 1 "if(1 == 1) return 1;"
assert 0 "if (1 == 2) return 1; else return 0;"
assert 4 "a = 2; b = 2; while (a == b) a = a + 2; return a;"
assert 2 "a = 1; while (a == 1) a = a + 1; return a;"
assert 3 "for(a=0; a<3; a=a+1) 1; return a;"
assert 14 "i = 0; for(a=0; a<3; a=a+1) {i = i + 1; i = i * 2;} return i;"
assert 2 "i = 0; if(i == 0) { i = 1; i = i * 2;} else {i = 3; i = i + 1;} return i;"
assert 1 "{i = 1; i = i + 1; i = i - 1;} return i;"
assert 5 "i=foo(); return i;" callfunc
assert 2 "i=0; i=foo2(2); return i;" callfunc
assert 5 "i=0; i=foo3(2, 3); return i;" callfunc
assert 21 "{i=0; i=foo4(1, 2, 3, 4, 5, 6); return i;}" callfunc
assert 1 "{i=0; foo5(); i=1; return i;}" callfunc

echo OK