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
    TK_IF,          // if
    TK_ELSE,        // else
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
    ND_BLOCK,   // ブロック
    ND_IF,      // if
} NodeKind;

typedef struct Node {
    NodeKind    kind;
    Node*       next;   // 次のstmtの内容

    Node*       lhs;
    Node*       rhs;
    
    Node*       body;   // body

    Node*       cond;
    Node*       then;
    
    int         val;
    int         offset;
} Node;

// 識別子リスト

typedef struct Function {
    Node* body;
    int stack_size;
} Function;


typedef enum {
    IDENT_FUNC,
    IDENT_LVAL,
} IdentType;

typedef struct Ident {
    IdentType   type;
    Ident*      next;
    char*       name;
    int         len;
    int         offset;
} Ident;

typedef enum {
    SC_GROBAL = 0,  // グローバルスコープ
    SC_FUNC,        // 関数スコープ
    SC_BLOCK,       // ブロックスコープ
} ScopeType;

typedef struct IdentScope{
    Ident*          idents;     // scope内で定義された識別子
    IdentScope*     parent;     // 親スコープへのポインタ
    ScopeType       type;       // スコープのタイプ
    int             stacksize;  // スコープで使うスタックのサイズ: SC_FUNCのときだけ使う
} IdentScope;

#endif
