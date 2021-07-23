#ifndef IDENT_MANAGER_H
#define IDENT_MANAGER_H

#include <rcc.h>

void id_begin_scope(ScopeType type);            // scopeの始まり
void id_end_scope();                            // scopeの終わり

Ident* id_declare_lvar(Token* tok, Type* type); // 変数の宣言

Ident* id_find_ident(Token* tok);               // 識別子を見つける
int id_get_curfunc_stack_size();                // 現在のスコープの関数のstackサイズ

#endif
