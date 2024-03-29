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

assert 3 'int main(){int p[3]; p[0] = 3; p[1] = 2; int *q; q = p + 1; return p[0];}'
assert 123 'int main(){int a[10]; a[0] = 3; a[1] = 4; a[1+2*2] = 123; return a[5];}'
assert 4 'int main(){int a[10]; a[0] = 3; a[1] = 4; return a[1];}'
assert 4 'int main(){int a[10]; *a = 3; *(a + 1) = 4; return *(a + 1);}'
assert 3 'int main() {int* p; tmalloc(&p); int* q; q = p + 3; return q - p;}'
assert 2 'int main() {int* p; tmalloc(&p); int* q; q = p + 1;return *q;}'
assert 18 'int main(){return foo() + 4;}'
assert 5 'int main(){int a;int *b; b = &a; *b = 5; return a;}'
assert 3 'int main(){int a; a=3; return a;}'
assert 1 'int main(){0; return 1; return 3; return 4;}'
assert 9 'int main(){return (3+5*( 9 - 6 ))   /2;}'
assert 20 'int main(){return (-10 + 20) * 2;}'
assert 1 'int main(){return (1 + 2 == 3) == 1;}'
assert 1 'int main(){return 2 > (1 * 3) - 2;}'
assert 8 'int main(){int a;a=3; int z;z=5; return a+z;}'
assert 3 'int main(){int foo;foo=3; return foo;}'
assert 8 'int main(){int foo123; int bar;foo123=3; bar=5; return foo123+bar;}'
assert 5 'int main(){{{{{int a; a=5; return a;}}}}}'
assert 10 'int main(){int a; a = 3; if(3 * 2 != 6) a = 4; else {a = 5;a = a + a;} return a;}'
assert 10 'int main(){int a;a = 3; while(a != 10){a = a + 1;} return a;}'
assert 2 'int main(){int a; int b;a = 0;b = 0;for(a = 1;a < 3; a = a + 1){b = b + 1;} return b;}'
assert 7 'int main(){return add(3,4);}'
assert 25 'int add2(int a,int b){return a + b;} int main(){return add2(10,15);}'
assert 3 'int main(){int a; int b;a = 3; b = &a; return *b;}'
assert 8 'int main(){int a; return sizeof(a);}'
assert 1 'int main() { int x;x=1; sizeof(x=2); return x; }'

echo OK