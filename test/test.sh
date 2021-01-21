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

assert 0 '0;'
assert 9 '(3+5*( 9 - 6 ))   /2;'
assert 20 '(-10 + 20) * 2;'
assert 1 '(1 + 2 == 3) == 1;'
assert 1 '2 > (1 * 3) - 2;'
assert 0 '3 <= 4 * 4 * (2 - 3);'
assert 3 'a=3; a;'
assert 8 'a=3; z=5; a+z;'
assert 3 'foo=3; foo;'
assert 8 'foo123=3; bar=5; foo123+bar;'


echo OK