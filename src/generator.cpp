#include <generator.h>
#include <pr_assm.h>

void gen_lval(Node* node);

void funcgen(Function* func){

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    // prologue
    printf("    push rbp\n");
    printf("    mov rbp, rsp\n");
    printf("    sub rsp, %d\n", func->stack_size);

    for(Node* node = func->body; node; node = node->next){
        gen(node);
    }

    // epilogue
    printf(".L.return:\n");
    printf("    mov rsp, rbp\n");
    printf("    pop rbp\n");
    printf("    ret\n");    
}


// 最後に答えを突っ込む
void gen(Node* node){
    
    switch(node->kind){
        case ND_RETURN:
            gen(node->lhs);
            printf("    pop rax\n");
            return;
        case ND_NUM:
            pa_push(node->val);
            return;
        case ND_LVAR:
            // gen_lval()で変数のoffsetをスタックにプッシュして、
            // それをraxレジスタにmov（＝ロード）してからpushする。
            gen_lval(node);
            pa_pop("rax");
            printf("    mov rax, [rax]\n");
            printf("    push rax\n");
            return;
        case ND_ASSIGN:
            // 左辺値 = 識別子のアドレス、右辺値の結果の順にスタックへ積む
            gen_lval(node->lhs);
            gen(node->rhs);

            printf("    pop rdi\n");            // 左辺値のアドレス
            printf("    pop rax\n");            // 右辺値の結果
            printf("    mov [rax], rdi\n");     // mov <-方向に
            printf("    push rdi\n");
            return;
        case ND_BLOCK:
        {
            Node* cur = node->body;
            while(cur){
                gen(cur);
                cur = cur->next;
            }
            return;
        }
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
        case ND_EQ:
            printf("    cmp rax, rdi\n");
            printf("    sete al\n");
            printf("    movzb rax, al\n");
            break;
        case ND_NE:
            printf("    cmp rax, rdi\n");
            printf("    setne al\n");
            printf("    movzb rax, al\n");
            break;
        case ND_LT:
            printf("    cmp rax, rdi\n");
            printf("    setl al\n");
            printf("    movzb rax, al\n");
            break;
        case ND_LE:
            printf("    cmp rax, rdi\n");
            printf("    setle al\n");
            printf("    movzb rax, al\n");
            break;
    }

    printf("    push rax\n");
}

void gen_lval(Node* node){
    if(node->kind != ND_LVAR){
        error("代入の左辺値が変数ではありません！");
    }

    printf("    mov rax, rbp\n");
    printf("    sub rax, %d\n", node->offset);
    printf("    push rax\n");
}