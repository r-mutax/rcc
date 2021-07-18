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

    CSrcFile* csrcfile = program();

    printf(".intel_syntax noprefix\n");

    Function* func = csrcfile->func;
    while(func){
        funcgen(func);
        func = func->next;
    }

    return 0;
}
