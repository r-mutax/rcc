
#include <tokenize.h>

struct Token *token;

// ===================
// inner func
// ===================
Token *new_token(TokenKind kind, Token *cur, char *str, int len);
bool startswith(char* p, const char* q);
static bool is_ident1(char c);
static bool is_ident2(char c);
static bool is_alnum(char c);
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

        if(is_ident1(*p)){
            char *st = p;
            // is_ident2がfalseになるまでポインタを進める。
            do{
                //fprintf(stderr, "%c", *p);
                p++;
            } while(is_ident2(*p));
            cur = cur->next = new_token(TK_IDENT, cur, st, abs(p - st));
            continue;
        }
        
        if(startswith(p, "==") || startswith(p, "!=")||
        startswith(p, "<=") || startswith(p, ">=")){
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
            continue;
        }

        if (strchr("+-*/()<>;={},&", *p)){
            cur = new_token(TK_RESERVED, cur, p++, 1);
            continue;
        }

        if ('a' <= *p && *p <= 'z'){
            cur = new_token(TK_IDENT, cur, p++, 1);
            continue;
        }

        if (isdigit(*p)){
            cur = new_token(TK_NUM, cur, p, 0);
            char* q = p;
            cur->val = strtol(p, &p, 10);
            cur->len = p - q;
            continue;
        }

        error_at(token->str, "トークナイズできません");
    }

    new_token(TK_EOF, cur, p, 0);

    token = head.next;

    return;
}

Token* new_token(TokenKind kind, Token *cur, char *str, int len){
    Token* tok = (Token*)calloc(1, sizeof(Token));

    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    cur->next = tok;
    return tok;
}

bool tk_consume(const char* op){
    if(strlen(op) != token->len
        || memcmp(token->str, op, token->len))
        return false;
    
    token = token->next;
    return true;
}

bool tk_consume(TokenKind kind){
    if(token->kind == kind){
        token = token->next;
        return true;
    }

    return false;
}

Token* tk_consume_ident(){
    if(token->kind != TK_IDENT){
        return NULL;
    }

    Token* tok = token;
    token = token->next;
    return tok;
}

Token* tk_consume_ident(const char* op){
    if(token->kind != TK_IDENT
        || strlen(op) != token->len
        || memcmp(token->str, op, token->len)){
        return NULL;
    }

    Token* tok = token;
    token = token->next;
    return tok;
}

Token* tk_expect_ident(){
    if(token->kind != TK_IDENT){
        error_at(token->str, "関数ではありません。");
        return NULL;
    }

    Token* tok = token;
    token = token->next;
    return tok;
}

Token* tk_expect_ident(const char* op){
    if(token->kind != TK_IDENT
        || strlen(op) != token->len
        || memcmp(token->str, op, token->len)){
        error_at(token->str, "関数ではありません。");
        return NULL;
    }

    Token* tok = token;
    token = token->next;
    return tok;
}

void tk_expect(const char* op){
    if(token->len != strlen(op)
        || memcmp(token->str, op, token->len))
        error_at(token->str, "'%s'ではありません。", op);

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

bool startswith(char* p, const char* q){
    return memcmp(p, q, strlen(q)) == 0;
}

static bool is_ident1(char c) {
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_';
}
static bool is_ident2(char c) {
  return is_ident1(c) || ('0' <= c && c <= '9');
}

static bool is_alnum(char c) {
    return is_ident2(c);
}

void tk_print(){
    char c = *(token->str);
    printf("%c",c);
}