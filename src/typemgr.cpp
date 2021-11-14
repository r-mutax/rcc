#include "typemgr.h"

Type*           cur_type;
Type            type_dict;
Type*           type_tail = &type_dict;

#define TYPE_NAME_INT   "int"

void ty_init(){

    // Provisional processing
    ty_add_type("int", 8, TYPE_INTEGER);
    ty_add_type("long", 8, TYPE_INTEGER);
}

void ty_add_type(const char* type_name, size_t size, TypeKind kind)
{
    Type* ty = (Type*) calloc(1, sizeof(Type));

    ty->kind = kind;
    ty->type_name = type_name;
    ty->size = size;

    type_tail->next = ty;
    type_tail = ty;
}

Type* ty_pointer_to(Type* base_type)
{
    if(base_type->pointer_from){
        return base_type->pointer_from;
    }

    Type* type = (Type*)calloc(1, sizeof(Type));

    type->kind = TYPE_POINTER;
    type->pointer_to = base_type;
    type->size = 8;
    base_type->pointer_from = type;

    return type;
}

Type* ty_array_of(Type* base_type, int array_size)
{
    Type* type = (Type*) calloc(1, sizeof(Type));

    type->kind = TYPE_ARRAY;
    type->array_of = base_type;
    type->size = base_type->size;
    type->array_size = array_size;

    return type;
}

Type* ty_get_type(const char* c, int len)
{
    for(Type* ty = type_dict.next; ty; ty = ty->next)
    {
        if(memcmp(c, ty->type_name, len) == 0){
            // find base type
            return ty;
        }
    }

    return NULL;
}

void ty_add_type(Node* node){
    // desicion node type
    // ex. add,sub,mul,div...           -> lhs type
    //     releational(equal, LE,LT...) -> integer
    //     variable address(& operator) -> pointer of lhs node
    //     dref                         -> lhs->type is pointer ? pointer from lhs : integer

    if(!node || node->type){
        return;
    }

    ty_add_type(node->lhs);
    ty_add_type(node->rhs);
    ty_add_type(node->cond);
    ty_add_type(node->then);
    ty_add_type(node->els);
    ty_add_type(node->init);
    ty_add_type(node->update);

    for(Node* n = node->body; n; n = n->next)
        ty_add_type(n);

    switch(node->kind){
        case ND_ADD:
        case ND_SUB:
        case ND_MUL:
        case ND_DIV:
        case ND_ASSIGN:
            node->type = node->lhs->type;
            break;
        case ND_EQ:
        case ND_NE:
        case ND_LT:
        case ND_LE:
        case ND_NUM:
        case ND_FUNC_CALL:
            node->type = ty_get_type("int", 3);
            break;
        case ND_ADDR:
            if(node->lhs->type->kind == TYPE_ARRAY){
                node->type = ty_pointer_to(node->lhs->type->array_of);
            } else {
                node->type = ty_pointer_to(node->lhs->type);
            }
            break;
        case ND_DEREF:
            if (node->lhs->type->kind == TYPE_POINTER){
                node->type = node->lhs->type->pointer_from;
            }
            else
                node->type = ty_get_type("int", 3);;
            break;
    }

    return;
}
