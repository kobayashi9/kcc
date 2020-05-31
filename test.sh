#!/bin/bash
assert() {
    expected="$1"
    input="$2"

    ./kcc "$input" > tmp.s
    cc -o tmp tmp.s
    ./tmp
    actual="$?"
    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

assert 21 sample/test1.ct
assert 41 sample/test2.ct
assert 47 sample/test3.ct

echo OK