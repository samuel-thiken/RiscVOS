
#include "sysapi.h"

int putchar(int ch)
{
	if (1 == (cons_write((char *) &ch, 1)))
		return ch;
	else return -1;
}
