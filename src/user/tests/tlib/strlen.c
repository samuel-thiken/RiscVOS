// See LICENSE for license details.

#include "sysapi.h"

size_t strlen(const char* s)
{
    const char *p = s;
    for (;; s++) {
        if (!*s) break;
    }
    return s - p;
}
