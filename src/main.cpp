#include <rcc.h>

#include <pr_assm.h>
#include <tokenize.h>
#include <parsing.h>
#include <generator.h>
#include <errmsg.h>

int main(int argc, char **argv){

    if(argc != 2) {
        fprintf(stderr, "引数の個数が正しくありません。");
        return 1;
    }

    reg_user_input(argv[1]);
    tk_tokenize(argv[1]);
    Node* node = expr();

    pa_headder();
    printf("main:\n");

    gen(node);

    pa_pop("rax");
    printf("    ret\n");

    return 0;
}
