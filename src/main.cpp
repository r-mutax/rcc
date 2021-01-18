#include <rcc.h>

#include <pr_assm.h>
#include <tokenize.h>
#include <parsing.h>
#include <generator.h>

// grobal buffer
extern Node* code[100];

int main(int argc, char **argv){

    if(argc != 2) {
        fprintf(stderr, "引数の個数が正しくありません。");
        return 1;
    }

    reg_user_input(argv[1]);
    tk_tokenize(argv[1]);
    program();

    pa_headder();
    printf("main:\n");

    // 変数26個分の領域を確保しておく
    printf("    push rbp\n");
    printf("    mov rbp, rsp\n");
    printf("    sub rsp, 208\n");


    for(int i = 0; code[i]; i++){
        gen(code[i]);

        printf("    pop rax\n");
    }

    printf("    mov rsp, rbp\n");
    printf("    pop rbp\n");
    printf("    ret\n");

    return 0;
}
