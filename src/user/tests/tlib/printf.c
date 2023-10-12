// See LICENSE for license details.

#include "sysapi.h"

int printf(const char* s, ...)
{
    int res = 0;
    va_list vl;
    va_start(vl, s);
    res = vprintf(s, vl);
    va_end(vl);
    return res;
}
