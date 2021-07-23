#include "typemgr.h"

Type*           cur_type;
Type           type_dict;

#define TYPE_NAME_INT   "int"

void ty_init(){

    Type*   int_type = (Type*)calloc(1, sizeof(Type));

    int_type->kind = TYPE_INT;
    int_type->type_name = TYPE_NAME_INT;

    type_dict.next = int_type;
}

Type* ty_pointer_to(Type* base_type)
{
    if(base_type->pointer_from){
        return base_type->pointer_from;
    }

    Type* type = (Type*)calloc(1, sizeof(Type));

    type->kind = TYPE_POINTER;
    type->pointer_to = base_type;
    base_type->pointer_from = type;

    return type;
}

Type* ty_get_type(char* c, int len)
{
    for(Type* ty = type_dict.next; ty; ty = ty->next)
    {
        if(memcmp(c, ty->type_name, len) == NULL){
            // find base type
            return ty;
        }
    }

    return NULL;
}
