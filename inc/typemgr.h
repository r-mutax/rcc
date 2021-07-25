#ifndef TYPEMGR
#define TYPEMGR
#include "rcc.h"

void ty_init();
Type* ty_pointer_to(Type* base_type);
Type* ty_get_type(const char* c, int len);
void ty_add_type(Node* node);

#endif