#ifndef TOKENIZE
#define TOKENIZE

// トークンの種類
typedef enum {
    TK_RESERVED,    // 記号
    TK_NUM,         // 整数トークン
    TK_IDENT,       // 識別子
    TK_EOF,         // 入力の終わりを表すトークン
} TokenKind;

// トークン構造体
typedef struct Token {
    TokenKind kind;
    Token   *next;
    int val;
    char *str;
    int len;
} Token;


// 汎用関数
bool tk_consume(const char* op);
void tk_expect(const char* op);
int tk_expect_number();
bool tk_at_eof();
void tk_tokenize(char *p);
Token* tk_consume_ident();

#endif
