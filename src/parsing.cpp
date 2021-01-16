#include <parsing.h>
#include <tokenize.h>
#include <rcc.h>

Node* new_node(NodeKind kind, Node* lhs, Node* rhs);
Node* new_node(int val);

Node* mul();
Node* primary();

// expr = mul ( "+" mul | "-" mul )*
Node* expr(){
    Node* node = mul();

    for(;;){
        if(tk_consume('+'))
            node = new_node(ND_ADD, node, mul());
        else if (tk_consume('-'))
            node = new_node(ND_SUB, node, mul());
        else
            return node;
    }
}

// mul = primary ( "*" primary | "/" primary )*
Node* mul(){
    Node* node = primary();

    for(;;){
        if(tk_consume('*'))
            node = new_node(ND_MUL, node, primary());
        else if(tk_consume('/'))
            node = new_node(ND_DIV, node, primary());
        else
            return node;        
    }
}

Node* primary(){
    if(tk_consume('(')){
        Node* node = expr();
        tk_expect(')');
        return node;
    }

    return new_node(tk_expect_number());
}

Node* new_node(NodeKind kind, Node* lhs, Node* rhs){
    Node* node = (Node*)calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node* new_node(int val){
    Node* node = (Node*)calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}