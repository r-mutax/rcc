#include <rcc.h>

#include <pr_assm.h>
#include <tokenize.h>

int main(int argc, char **argv){

    if(argc != 2) {
        fprintf(stderr, "引数の個数が正しくありません。");
        return 1;
    }

    tk_tokenize(argv[1]);

    pa_headder();
    printf("main:\n");
    printf("    mov rax, %d\n", tk_expect_number());

    while(!tk_at_eof()) {
        if(tk_consume('+')) {
            printf("    add rax, %d\n", tk_expect_number());
            continue;
        }

        tk_expect('-');
        printf("    sub rax, %d\n", tk_expect_number());
    }

    printf("    ret\n");

    return 0;
}
