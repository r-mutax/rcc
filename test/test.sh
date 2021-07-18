#!/bin/bash

assert(){
    expected="$1"
    input="$2"

    ./bin/rcc "$input" > ./test/tmp.s
    cc -static -o ./test/tmp ./test/tmp.s ./test/foo.o 
    ./test/tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

assert 1 'main(){0; return 1; return 3; return 4;}'
assert 9 'main(){return (3+5*( 9 - 6 ))   /2;}'
assert 20 'main(){return (-10 + 20) * 2;}'
assert 1 'main(){return (1 + 2 == 3) == 1;}'
assert 1 'main(){return 2 > (1 * 3) - 2;}'
assert 0 'main(){return 3 <= 4 * 4 * (2 - 3);}'
assert 3 'main(){a=3; return a;}'
assert 8 'main(){a=3; z=5; return a+z;}'
assert 3 'main(){foo=3; return foo;}'
assert 8 'main(){foo123=3; bar=5; return foo123+bar;}'
assert 8 'main(){{foo123=3; bar=5; return foo123+bar;}}'
assert 5 'main(){{{{{a=5; return a;}}}}}'
assert 3 'main(){a = 3; if(3 * 2 != 6) a = 4; return a;}'
assert 10 'main(){a = 3; if(3 * 2 != 6) a = 4; else {a = 5;a = a + a;} return a;}'
assert 10 'main(){a = 3; while(a != 10){a = a + 1;} return a;}'
assert 2 'main(){a = 0;b = 0;for(a = 1;a < 3; a = a + 1){b = b + 1;} return b;}'
assert 18 'main(){return foo() + 4;}'
assert 7 'main(){return add(3,4);}'
assert 25 'add2(a,b){return a + b;} main(){return add2(10,15);}'

echo OK