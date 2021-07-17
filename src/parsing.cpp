
#include "parsing.h"
#include "tokenize.h"
#include "ident_manager.h"

// local
Node* new_node(NodeKind kind, Node* lhs, Node* rhs);
Node* new_node(int val);

// grammer
Node* compound_stmt();
Node* stmt();
Node* expr();
Node* assign();
Node* equality();
Node* relational();
Node* add();
Node* mul();
Node* primary();
Node* unary();


// program = stmt*
// ひとまず入力全体で一つのmain関数としてコンパイルする。
Function* program(){
    Node head = {};
    Node* cur = &head;

    // 新しい関数スコープを作る
    id_begin_scope(SC_FUNC);

    // ここではnodeを数珠つなぎにしていく
    while(!tk_at_eof()){
        cur->next = stmt();
        cur = cur->next;
    }

    Function* func = (Function*) calloc(1, sizeof(Function));
    // bodyから伸びるnodeのチェーンが関数のstmtの並びになっている。
    func->body = head.next;

    // stack_sizeの計算
    func->stack_size = id_get_curfunc_stack_size();

    // スコープを抜ける
    id_end_scope();

    return func;
}

// compound_stmt = stmt* "}"
Node* compound_stmt(){

    Node head = {};
    Node* cur = &head;

    while(!tk_consume("}")){
        cur = cur->next = stmt();
    }

    Node* node = (Node*)calloc(1, sizeof(Node));
    node->kind = ND_BLOCK;
    node->body = head.next;
    return node;
}

// stmt = expr ';' | 'return' expr | 'if (' expr() ')' | '{' compound_stmt
Node* stmt(){
    Node* node;

    if(tk_consume(TK_RETURN)){
        node = (Node*)calloc(1, sizeof(Node));
        node->kind = ND_RETURN;
        node->lhs = expr();
    } else if(tk_consume(TK_IF)){
        node = (Node*)calloc(1, sizeof(Node));
        node->kind = ND_IF;

        tk_expect("(");
        node->cond = expr();
        tk_expect(")");

        node->then = stmt();
        return node;
    } else if(tk_consume("{")){
        return compound_stmt();
    } else {
        node = expr();
    }

    tk_expect(";");
    return node;
}

// assign = equality ( '=' assign) ?
Node* assign(){
    Node* node = equality();

    if(tk_consume("="))
        node = new_node(ND_ASSIGN, node, assign());
    
    return node;
}

// expr = equality()
Node* expr(){
    return assign();
}

// equality = relational ( "==" relational | "!=" relational )*
Node* equality(){
    Node* node = relational();

    for(;;){
        if(tk_consume("=="))
            node = new_node(ND_EQ, node, relational());
        else if(tk_consume("!="))
            node = new_node(ND_NE, node, relational());
        else
            return node;
    }
}

// relational = add ( "<" add | "<=" add)*
Node* relational(){
    Node* node = add();

    for(;;){
        if(tk_consume("<"))
            node = new_node(ND_LT, node, add());
        else if(tk_consume("<="))
            node = new_node(ND_LE, node, add());
        else if(tk_consume(">"))
            node = new_node(ND_LT, add(), node);
        else if(tk_consume(">="))
            node = new_node(ND_LE, add(), node);
        else
            return node;
    }
}

// add = mul ( "+" mul | "-" mul )*
Node* add(){
    Node* node = mul();

    for(;;){
        if(tk_consume("+"))
            node = new_node(ND_ADD, node, mul());
        else if (tk_consume("-"))
            node = new_node(ND_SUB, node, mul());
        else
            return node;
    }
}

// mul = primary ( "*" unary | "/" unary )*
Node* mul(){
    Node* node = unary();

    for(;;){
        if(tk_consume("*"))
            node = new_node(ND_MUL, node, unary());
        else if(tk_consume("/"))
            node = new_node(ND_DIV, node, unary());
        else
            return node;        
    }
}

// unary = ("+" | "-")? primary
Node* unary(){
    if(tk_consume("+")){
        return primary();
    }

    if(tk_consume("-")){
        Node* node = new_node(ND_SUB, new_node(0), primary());
        return node;
    }

    return primary();
}

// primary = num | ("(" expr ")")
Node* primary(){
    if(tk_consume("(")){
        Node* node = expr();
        tk_expect(")");
        return node;
    }

    // 識別子トークンだった場合
    Token* tok = tk_consume_ident();
    if(tok){
        // 現時点では変数だけ
        Node* node = (Node*)calloc(1, sizeof(Node));
        node->kind = ND_LVAR;

        Ident* ident = id_find_ident(tok);

        if(ident){
            node->offset = ident->offset;
        } else {
            ident = id_declare_lvar(tok);
            node->offset = ident->offset;
        }
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
