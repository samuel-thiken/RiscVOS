// See LICENSE for license details.

#include "sysapi.h"

int puts(const char *s)
{
	size_t len = strlen(s) + 1; // +1 pour le \n
	char *out = __builtin_alloca(len);
	strncpy(out, s, len - 1);
	out[len - 1] = '\n';
	cons_write(out, len);
	return 0;
}
