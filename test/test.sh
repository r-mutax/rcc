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

assert 1 '{0; return 1; return 3; return 4;}'
assert 9 '{return (3+5*( 9 - 6 ))   /2;}'
assert 20 '{return (-10 + 20) * 2;}'
assert 1 '{return (1 + 2 == 3) == 1;}'
assert 1 '{return 2 > (1 * 3) - 2;}'
assert 0 '{return 3 <= 4 * 4 * (2 - 3);}'
assert 3 '{a=3; return a;}'
assert 8 '{a=3; z=5; return a+z;}'
assert 3 '{foo=3; return foo;}'
assert 8 '{foo123=3; bar=5; return foo123+bar;}'
assert 8 '{{foo123=3; bar=5; return foo123+bar;}}'
assert 5 '{{{{{a=5; return a;}}}}}'

echo OK