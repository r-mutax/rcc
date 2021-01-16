#include <generator.h>
#include <pr_assm.h>

void gen(Node* node){
    if(node->kind == ND_NUM){
        pa_push(node->val);
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    pa_pop("rdi");
    pa_pop("rax");

    switch(node->kind){
        case ND_ADD:
            printf("    add rax, rdi\n");
            break;
        case ND_SUB:
            printf("    sub rax, rdi\n");
            break;
        case ND_MUL:
            printf("    imul rax, rdi\n");
            break;
        case ND_DIV:
            printf("    cqo\n");
            printf("    idiv rdi\n");
            break;
    }

    printf("    push rax\n");
}