#ifndef TYPEMGR
#define TYPEMGR
#include "rcc.h"

void ty_init();
Type* ty_pointer_to(Type* base_type);
Type* ty_get_type(char* c, int len);

#endif