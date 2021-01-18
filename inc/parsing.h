#ifndef PARSING
#define PARSING

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
} NodeKind;

typedef struct Node {
    NodeKind    kind;
    Node*       lhs;
    Node*       rhs;
    int         val;
    int         offset;
} Node;

void program();
Node** pr_getNode();

#endif