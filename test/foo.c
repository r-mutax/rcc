#include <stdio.h>
#include <stdlib.h>

int foo(){
    return 14;
}

int add(int a, int b){
    return a + b;
}

void tmalloc(int** p){
    long *q = malloc(sizeof(long) * 4);
    q[0] = 1;
    q[1] = 2;
    q[2] = 3;
    q[3] = 4;
    
    *p = (int*)q;
    return;
}