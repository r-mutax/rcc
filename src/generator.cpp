#include <generator.h>
#include <pr_assm.h>

static void gen_lval(Node* node);
static void gen_compound_stmt(Node* node);
static void gen_stmt(Node* node);

static int cnt_if = 0;

// function generator
void funcgen(Function* func){

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    // prologue
    printf("    push rbp\n");
    printf("    mov rbp, rsp\n");
    printf("    sub rsp, %d\n", func->stack_size);

    // all the stmt is include by compound stmt.
    gen_compound_stmt(func->body);

    // epilogue
    printf(".L.return:\n");
    printf("    mov rsp, rbp\n");
    printf("    pop rbp\n");
    printf("    ret\n");    
}

// generator of compound stmt
static void gen_compound_stmt(Node* node)
{
    switch(node->kind){
        case ND_BLOCK:
            for(Node* cur = node->body; cur; cur = cur->next){
                gen_stmt(cur);
            }
        return;
    }
}

// generator of stmt(return, block, other expr -> gen())
static void gen_stmt(Node* node){
    switch(node->kind){
        case ND_RETURN:
            gen(node->lhs);
            printf("    pop rax\n");
            printf("    jmp .L.return\n");
            return;
        case ND_BLOCK:
            // blockの中にblockが来たとき
            gen_compound_stmt(node);
            break;
        case ND_IF:
            gen(node->cond);

            pa_pop("rax");
            printf("    cmp rax, 0\n");
            printf("    je .L.else%d\n", cnt_if);
            gen_stmt(node->then);
            printf("    jmp .L.else%d\n", cnt_if);
            printf(".L.else%d:\n", cnt_if);

            if(node->els){
                gen_stmt(node->els);
            }
            printf(".L.if_end%d:\n", cnt_if);

            cnt_if++;
            break;
        case ND_WHILE:
            printf(".L.while%d:\n", cnt_if);
            
            // condition check
            gen(node->cond);
            pa_pop("rax");
            printf("    cmp rax, 0\n");
            printf("    je .L.end%d\n", cnt_if);
            
            gen_stmt(node->then);
            printf("    jmp .L.while%d\n", cnt_if);
            printf(".L.end%d:\n", cnt_if);

            cnt_if++;
            break;
        case ND_FOR:
            gen(node->init);
            
            printf(".L.for%d:\n", cnt_if);

            // condition check
            gen(node->cond);
            pa_pop("rax");
            printf("    cmp rax, 0\n");
            printf("    je .L.end%d\n", cnt_if);
            
            // then
            gen_stmt(node->then);
            
            // update
            gen(node->update);
            printf("    jmp .L.for%d\n", cnt_if);
            printf(".L.end%d:\n", cnt_if);

            cnt_if++;
            break;
        default:
            gen(node);
            break;
    }
}

// assembly generator
// ----------------------
// end of this func, stored to stack rax register
void gen(Node* node){
    
    // if primary immidiate return.
    switch(node->kind){
        case ND_NUM:
            pa_push(node->val);
            return;
        case ND_FUNC_CALL:
            printf("    call %s\n", node->func);
            printf("    push rax\n");
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
    }

    // right side ans left side assemble each. 
    gen(node->lhs);
    gen(node->rhs);

    // pop lhs and rhs
    pa_pop("rdi");
    pa_pop("rax");

    // calculate and store result to stack top.
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

// generate value address in stack
void gen_lval(Node* node){
    if(node->kind != ND_LVAR){
        error("代入の左辺値が変数ではありません！");
    }

    printf("    mov rax, rbp\n");
    printf("    sub rax, %d\n", node->offset);
    printf("    push rax\n");
}