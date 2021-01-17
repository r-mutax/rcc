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
} NodeKind;

typedef struct Node {
    NodeKind    kind;
    Node*       lhs;
    Node*       rhs;
    int         val;
} Node;


Node* expr();

#endif