#include <rcc.h>
#include <errmsg.h>
#include <tokenize.h>

struct Token *token;

// ===================
// inner func
// ===================
Token *new_token(TokenKind kind, Token *cur, char *str);


// ===================
// func implement
// ===================
void tk_tokenize(char *p){
    Token head;
    head.next = NULL;
    Token* cur = &head;

    while(*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (strchr("+-*/()", *p)){
            cur = new_token(TK_RESERVED, cur, p++);
            continue;
        }

        if (isdigit(*p)){
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10);
            continue;
        }

        error_at(token->str, "トークナイズできません");
    }

    new_token(TK_EOF, cur, p);

    token = head.next;

    return;
}

Token* new_token(TokenKind kind, Token *cur, char *str){
    Token* tok = (Token*)calloc(1, sizeof(Token));

    tok->kind = kind;
    tok-> str = str;
    cur->next = tok;
    return tok;
}

bool tk_consume(char op){
    if(token->kind != TK_RESERVED || token->str[0] != op)
        return false;
    
    token = token->next;
    return true;
}

void tk_expect(char op){
    if(token->kind != TK_RESERVED || token->str[0] != op)
        error_at(token->str, "'%c'ではありません。", op);

    token = token->next;
}

int tk_expect_number(){
    if(token->kind != TK_NUM)
        error_at(token->str, "数値ではありません");

    int val = token->val;
    token = token->next;
    return val;
}

bool tk_at_eof(){
    return token->kind == TK_EOF;
}