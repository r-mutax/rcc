
#include "parsing.h"
#include "tokenize.h"
#include "ident_manager.h"
#include "typemgr.h"

// local
Node* new_node(NodeKind kind, Node* lhs, Node* rhs);
Node* new_node(int val);

// grammer
Node* compound_stmt();
Node* stmt();
Node* declare(Token* tok_type);
Node* expr();
Node* assign();
Node* equality();
Node* relational();
Node* add();
Node* mul();
Node* primary();
Node* unary();
Function*   func();


// program = stmt*
// ひとまず入力全体で一つのmain関数としてコンパイルする。
CSrcFile* program(){

    ty_init();

    Function head = {};
    Function* cur = &head;

    int n = 0;
    while(!tk_at_eof()){
        cur->next = func();
        cur = cur->next;
    }

    CSrcFile* csrcfile = (CSrcFile*) calloc(1, sizeof(CSrcFile));
    csrcfile->func = head.next;

    return csrcfile;
}

Function*   func(){

    // type
    Token* tok_retval_type = tk_expect_type();

    // funcname
    Token* tok_funcname = tk_expect_ident();
    tk_expect("(");

    id_begin_scope(SC_FUNC);

    Function* func = (Function*) calloc(1, sizeof(Function));

    // paramater definition
    while(!tk_consume(")")){
        // first, type definition.
        Token* tok_type = tk_expect_type();
        Node* node = declare(tok_type);
        if(node){
            func->paramater_num++;
        }

        tk_consume(",");
    }

    // func body
    tk_expect("{");    
    func->body = compound_stmt();
    func->funcname = (char*)malloc(tok_funcname->len + 1);
    memcpy(func->funcname, tok_funcname->str, tok_funcname->len);
    func->funcname[tok_funcname->len] = '\0';

    // stack_sizeの計算
    func->stack_size = id_get_curfunc_stack_size();

    // スコープを抜ける
    id_end_scope();

    return func;
}

// compound_stmt = stmt* "}"
Node* compound_stmt(){

    id_begin_scope(SC_BLOCK);

    Node head = {};
    Node* cur = &head;

    while(!tk_consume("}")){
        cur = cur->next = stmt();
        ty_add_type(cur);
    }

    Node* node = (Node*)calloc(1, sizeof(Node));
    node->kind = ND_BLOCK;
    node->body = head.next;

    id_end_scope();

    return node;
}

// stmt = expr ';' 
//       | 'return' expr 
//       | 'if (' expr() ')' ('else' stmt)?  
//       | '{' compound_stmt
Node* stmt(){
    Node* node;
    if(tk_consume("return")){
        node = (Node*)calloc(1, sizeof(Node));
        node->kind = ND_RETURN;
        node->lhs = expr();
    } else if(tk_consume("if")){
        node = (Node*)calloc(1, sizeof(Node));
        node->kind = ND_IF;
        tk_expect("(");
        node->cond = expr();
        tk_expect(")");
        node->then = stmt();

        if(tk_consume("else")){
            node->els = stmt();
        }
        return node;
    } else if(tk_consume("while")){
        node = (Node*) calloc(1, sizeof(Node));
        node->kind = ND_WHILE;
        
        tk_expect("(");
        node->cond = expr();
        tk_expect(")");

        node->then = stmt();
        return node;
    }else if(tk_consume("for")){
        node = (Node*) calloc(1, sizeof(Node));
        node->kind = ND_FOR;

        tk_expect("(");
    
        if(!tk_consume(";")){
            node->init = expr();
            tk_expect(";");
        } 
        if(!tk_consume(";")){
            node->cond = expr();
            tk_expect(";");
        }

        if(!tk_consume(")")){
            node->update = expr();
            tk_expect(")");
        }

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

// declare = "int" "*"* identify
Node* declare(Token* tok_type){

    // judge type(pointer?)
    Type* type = ty_get_type(tok_type->str, tok_type->len);
    while(tk_consume("*")){
        type = ty_pointer_to(type);
    }

    // ecpect lvar identifer
    Token* tok = tk_expect_ident();
    Node* node = (Node*)calloc(1, sizeof(Node));
    node->kind = ND_LVAR;
    
    Ident* ident = id_find_ident(tok);
    if(ident){
        error_at(tok->str, "'%s' is redifiniton.", ident->name);
    } else {
        ident = id_declare_lvar(tok, type);
    }

    node->offset = ident->offset;

    return node;
}

// expr = equality()
Node* expr(){
    if(Token* tok = tk_consume_type()){
        return declare(tok);
    }
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

    // num add num          -> ok
    // pointer add pointer  -> ng
    // pointer add num      -> ok
    // add <-> sub

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

// unary = (("+" | "-")? primary) | (("&" | "*")? unary)
Node* unary(){
    if(tk_consume("+")){
        return primary();
    }

    if(tk_consume("-")){
        Node* node = new_node(ND_SUB, new_node(0), primary());
        return node;
    }

    if(tk_consume("*")){
        Node* node = new_node(ND_DEREF, unary(), NULL);
        return node;
    }

    if(tk_consume("&")){
        Node* node = new_node(ND_ADDR, unary(), NULL);
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
        if(tk_consume("(")){
            // func call
            Node* node = (Node*)calloc(1, sizeof(Node));
            node->kind = ND_FUNC_CALL;
            node->func = (char*)malloc(tok->len + 1);
            memcpy(node->func, tok->str, tok->len);
            node->func[tok->len] = '\0';

            // until ")", arguments
            int arguments = 0;

            Node head = {};
            Node* cur = &head;
            while(!tk_consume(")")){
                if(arguments == 6){
                    error_at(tok->str, "Arguments is too many.\n");
                }

                cur = cur->next = expr(); 
                tk_consume(",");
                arguments++;
            }
            node->arguments = head.next;

            tk_consume(")");
            return node;
        }

        // 現時点では変数だけ
        Node* node = (Node*)calloc(1, sizeof(Node));
        node->kind = ND_LVAR;

        Ident* ident = id_find_ident(tok);

        if(ident){
            node->offset = ident->offset;
            node->type = ident->type;
        } else {
            char* ident_name = (char*) calloc(1, tok->len);
            memcpy(ident_name, tok->str, tok->len);
            error_at(tok->str, "'%s' is no declared in this scope.", ident_name);
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
