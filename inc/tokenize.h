#ifndef TOKENIZE
#define TOKENIZE

#include <rcc.h>

// 汎用関数
bool tk_consume(const char* op);
bool tk_consume(TokenKind kind);
void tk_expect(const char* op);
int tk_expect_number();
bool tk_at_eof();
void tk_tokenize(char *p);
Token* tk_consume_ident();
bool tk_consume_return();

#endif
