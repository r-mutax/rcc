#include "ident_manager.h"

int func_id = 0;
IdentScope      global_scope;

// grobal_scopeを初期値とする
IdentScope      *sc_cur = &global_scope;


// ------------------------------
// ローカル関数
// ------------------------------
static IdentScope* lid_get_func_scope();

// スコープに入る
void id_begin_scope(ScopeType type){
    IdentScope*     iscope = (IdentScope*)calloc(1, sizeof(IdentScope));
    iscope->parent = sc_cur;
    iscope->type = type;
    sc_cur = iscope;
    return;
}

// スコープを抜ける
void id_end_scope(){
    sc_cur = sc_cur->parent;
    return;
}

// 変数宣言を、現在のスコープの変数リストに追加する。
Ident* id_declare_lvar(Token* tok, Type* type){
    Ident* ident = (Ident*) calloc(1,sizeof(Ident));

    ident->kind = IDENT_LVAL;
    ident->name = tok->str;
    ident->len = tok->len;
    ident->type = type;

    // 関数で使用するスタックサイズを増やす
    IdentScope* funcscope = lid_get_func_scope();
    funcscope->stacksize += 8;

    // オフセットは、関数で使用するスタックサイズの一番最後になる。
    ident->offset = funcscope->stacksize;

    // 現在のスコープの変数リストに引っ付ける。
    ident->next = sc_cur->idents;
    sc_cur->idents = ident;

    return ident;
}

// 関数スコープを手に入れる
static IdentScope* lid_get_func_scope(){
    IdentScope* funcscope = sc_cur;

    do {
        if(funcscope->type == SC_FUNC)
            break;
        
        funcscope = funcscope->parent;
        
        if(funcscope->type == SC_GROBAL)
            error("[error 1453] unexpected error.");
    } while(1);
    
    return funcscope;
}

// 識別子を見つける
Ident* id_find_ident(Token* tok){

    // explain
    // 一番下層のscopeから親のscopeまで登っていき、
    // 見つかったらそれを識別子にする。

    for(IdentScope* iscope = sc_cur; iscope; iscope = iscope->parent){
        for(Ident* ident = iscope->idents; ident; ident = ident->next){
            if(ident->len = tok->len && !memcmp(tok->str, ident->name, tok->len)){
                return ident;
            }
        }
    }  
    return NULL;
}

int id_get_curfunc_stack_size(){
    IdentScope* funcscope = lid_get_func_scope();

    return funcscope->stacksize;
}
