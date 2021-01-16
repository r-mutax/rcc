#ifndef TOKENIZE
#define TOKENIZE

// トークンの種類
typedef enum {
    TK_RESERVED,    // 記号
    TK_NUM,         // 整数トークン
    TK_EOF,         // 入力の終わりを表すトークン
} TokenKind;

// トークン構造体
typedef struct Token {
    TokenKind kind;
    Token   *next;
    int val;
    char *str;
} Token;


// 汎用関数
bool tk_consume(char op);
void tk_expect(char op);
int tk_expect_number();
bool tk_at_eof();
void tk_tokenize(char *p);

#endif
