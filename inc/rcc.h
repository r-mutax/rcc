#ifndef RCC
#define RCC

// C標準ライブラリ
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// プロジェクト内ヘッダ
#include "errmsg.h"

// ===============================
//  tokenizer
// ===============================
// トークンの種類
typedef enum {
    TK_RESERVED,    // 記号
    TK_NUM,         // 整数トークン
    TK_IDENT,       // 識別子
    TK_EOF,         // 入力の終わりを表すトークン
    TK_RETURN,      // return
} TokenKind;

// トークン構造体
typedef struct Token {
    TokenKind kind;
    Token   *next;
    int val;
    char *str;
    int len;
} Token;

// ===============================
//  parser
// ===============================
typedef enum {
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_NUM,
    ND_EQ,      // ==
    ND_NE,      // !=
    ND_LT,      // <
    ND_LE,      // >
    ND_ASSIGN,  // 代入式
    ND_LVAR,    // ローカル変数
    ND_RETURN,  // return
} NodeKind;

typedef struct Node {
    NodeKind    kind;
    Node*       next;   // 次のstmtの内容

    Node*       lhs;
    Node*       rhs;
    int         val;
    int         offset;
} Node;

// 識別子リスト
typedef struct LVar {
    LVar* next;     // 次の識別子
    char* name;     // 名前
    int len;        // 名前の長さ
    int offset;     // RBPからのオフセット
} LVar;

typedef struct Function {
    Node* body;
    LVar* locals;
    int stack_size;
} Function;

#endif