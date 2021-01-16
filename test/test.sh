#!/bin/bash

assert(){
    expected="$1"
    input="$2"

    ./bin/rcc "$input" > ./test/tmp.s
    cc -o ./test/tmp ./test/tmp.s
    ./test/tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

assert 0 0
assert 9 '(3+5*( 9 - 6 ))   /2'
assert 20 '(-10 + 20) * 2'

echo OK