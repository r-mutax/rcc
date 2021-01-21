
#include "parsing.h"
#include "tokenize.h"

// local
Node* new_node(NodeKind kind, Node* lhs, Node* rhs);
Node* new_node(int val);

// grammer
Node* stmt();
Node* expr();
Node* assign();
Node* equality();
Node* relational();
Node* add();
Node* mul();
Node* primary();
Node* unary();

LVar* find_lvar(Token* tok);

// grobal buffer
LVar* locals;

// program = stmt*
// ひとまず入力全体で一つのmain関数としてコンパイルする。
Function* program(){
    Node head = {};
    Node* cur = &head;

    locals = nullptr;
    // ここではnodeを数珠つなぎにしていく
    while(!tk_at_eof()){
        cur->next = stmt();
        cur = cur->next;
    }

    Function* func = (Function*) calloc(1, sizeof(Function));
    // bodyから伸びるnodeのチェーンが関数のstmtの並びになっている。
    func->body = head.next;
    func->locals = locals;

    // stack_sizeの計算
    if(func->locals != NULL){
        // locals != NULLのときはローカル変数がある。
        func->stack_size = locals->offset;
    }

    return func;
}

// stmt = expr ';'
Node* stmt(){
    Node* node = expr();
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

        // localsにすでに登録済の変数か？
        LVar* lvar = find_lvar(tok);
        if(lvar){
            // 登録済の場合
            node->offset = lvar->offset;
        } else {
            // 初検出ならLVarを追加する
            lvar = (LVar*)calloc(1, sizeof(LVar));

            // localsが一番最後の変数になることに注意！
            lvar->next = locals;
            lvar->name = tok->str;
            lvar->len = tok->len;
            lvar->offset = locals->offset + 8;
            node->offset = lvar->offset;
            locals = lvar;
        }
        node->offset = (tok->str[0] - 'a' + 1) * 8;
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


LVar* find_lvar(Token* tok){
    if(locals == nullptr){
        locals = (LVar*)calloc(1, sizeof(LVar));
    }

    for(LVar *var = locals; var; var = var->next){
        if(var->len == tok->len && !memcmp(tok->str, var->name, var->len)){
            return var;
        }
    }
    return NULL;
}