#ifndef ERRMSG
#define ERRMSG

#include <rcc.h>

void error(const char* fmt, ...);
void error_at(char *loc, const char* fmt, ...);
void reg_user_input(char* p);

#endif
