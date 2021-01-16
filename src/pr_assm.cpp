#include <pr_assm.h>

void pa_headder(){
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
}

void pa_push(int val){
    printf("    push %d\n", val);
}

void pa_pop(const char* reg){
    printf("    pop %s\n", reg);
}