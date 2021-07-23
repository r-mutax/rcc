#ifndef TOKENIZE
#define TOKENIZE

#include <rcc.h>

// 汎用関数
void tk_tokenize(char *p);


bool tk_consume(const char* op);
bool tk_consume(TokenKind kind);
void tk_expect(const char* op);

Token* tk_consume_ident();
Token* tk_consume_ident(const char* op);
Token* tk_expect_ident();
Token* tk_expect_ident(const char* op);

Token* tk_expect_type();
Token* tk_consume_type();

int tk_expect_number();
bool tk_at_eof();
void tk_print();

#endif
