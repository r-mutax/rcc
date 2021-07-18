
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
Function*   func();


// program = stmt*
// ひとまず入力全体で一つのmain関数としてコンパイルする。
CSrcFile* program(){

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

    // funcname
    Token* tok_funcname = tk_expect_ident();
    tk_expect("(");

    id_begin_scope(SC_FUNC);

    Function* func = (Function*) calloc(1, sizeof(Function));

    // paramater definition
    while(!tk_consume(")")){
        Token* tok_lvar = tk_consume_ident();
        if(tok_lvar){
            Node* lvar_node = (Node*)calloc(1, sizeof(Node));

            Ident* ident = id_find_ident(tok_lvar);
            if(ident){
                // error, redifinition paramater.
                error_at(tok_lvar->str, "再定義されました。");
            } else {
                // add paramater as local variable.
                ident = id_declare_lvar(tok_lvar);
                func->paramater_num++;
            }
            tk_consume(",");
        }
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

// stmt = expr ';' 
//       | 'return' expr 
//       | 'if (' expr() ')' ('else' stmt)?  
//       | '{' compound_stmt
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

        if(tk_consume(TK_ELSE)){
            node->els = stmt();
        }
        return node;
    } else if(tk_consume(TK_WHILE)){
        node = (Node*) calloc(1, sizeof(Node));
        node->kind = ND_WHILE;
        
        tk_expect("(");
        node->cond = expr();
        tk_expect(")");

        node->then = stmt();
        return node;
    }else if(tk_consume(TK_FOR)){
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
