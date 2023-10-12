/*
 * Copyright (c) 1994-1996 University of Utah and the Flux Group.
 * All rights reserved.
 * 
 * This file is part of the Flux OSKit.  The OSKit is free software, also known
 * as "open source;" you can redistribute it and/or modify it under the terms
 * of the GNU General Public License (GPL), version 2, as published by the Free
 * Software Foundation (FSF).  To explore alternate licensing terms, contact
 * the University of Utah at csl-dist@cs.utah.edu or +1-801-585-3271.
 * 
 * The OSKit is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GPL for more details.  You should have
 * received a copy of the GPL along with the OSKit; see the file COPYING.  If
 * not, write to the FSF, 59 Temple Place #330, Boston, MA 02111-1307, USA.
 */
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */

#include "stdarg.h"
#include "stddef.h"

#define SPRINTF_UNLIMITED 0xFFFFFFFu
struct sprintf_state {
	char *buf;
	unsigned int len;
	unsigned int max;
};


int vsnprintf(char *out, size_t n, const char *s, va_list vl)
{
	int format = 0;
	int longarg = 0;
	size_t pos = 0;
	for (; *s; s++)
	{
		if(format)
		{
			switch (*s)
			{
				case 'l':
				{
					longarg = 1;
					break;
				}
				case 'p':
				{
					longarg = 1;
					if(out && pos < n)
					{
						out[pos] = '0';
					}
					pos++;
					if(out && pos < n)
					{
						out[pos] = 'x';
					}
					pos++;
				}
				case 'x':
				{
					long num = longarg ? va_arg(vl,
					long) : va_arg(vl,
					int);
					int hexdigits = 2 * (longarg ? sizeof(long) : sizeof(int)) - 1;
					for (int i = hexdigits; i >= 0; i--)
					{
						int d = (num >> (4 * i)) & 0xF;
						if(out && pos < n)
						{
							out[pos] = (d < 10 ? '0' + d : 'a' + d - 10);
						}
						pos++;
					}
					longarg = 0;
					format = 0;
					break;
				}
				case 'u':
				{
					unsigned long num = longarg ? va_arg(vl,
					unsigned long) : va_arg(vl,
					unsigned int);
					long digits = 1;
					for (long nn = num; nn /= 10; digits++);
					for (int i = digits - 1; i >= 0; i--)
					{
						if(out && pos + i < n)
						{
							out[pos + i] = '0' + (num % 10);
						}
						num /= 10;
					}
					pos += digits;
					longarg = 0;
					format = 0;
					break;
				}
				case 'd':
				{
					long num = longarg ? va_arg(vl,
					long) : va_arg(vl,
					int);
					if(num < 0)
					{
						num = -num;
						if(out && pos < n)
						{
							out[pos] = '-';
						}
						pos++;
					}
					long digits = 1;
					for (long nn = num; nn /= 10; digits++);
					for (int i = digits - 1; i >= 0; i--)
					{
						if(out && pos + i < n)
						{
							out[pos + i] = '0' + (num % 10);
						}
						num /= 10;
					}
					pos += digits;
					longarg = 0;
					format = 0;
					break;
				}
				case 's':
				{
					const char *s2 = va_arg(vl,
					const char*);
					while (*s2)
					{
						if(out && pos < n)
						{
							out[pos] = *s2;
						}
						pos++;
						s2++;
					}
					longarg = 0;
					format = 0;
					break;
				}
				case 'c':
				{
					if(out && pos < n)
					{
						out[pos] = (char) va_arg(vl,
						int);
					}
					pos++;
					longarg = 0;
					format = 0;
					break;
				}
				default:
					break;
			}
		} else if(*s == '%')
		{
			format = 1;
		} else
		{
			if(out && pos < n)
			{
				out[pos] = *s;
			}
			pos++;
		}
	}
	if(out && pos < n)
	{
		out[pos] = 0;
	} else if(out && n)
	{
		out[n - 1] = 0;
	}
	return pos;
}

int vsprintf(char *s, const char *fmt, va_list args)
{
	return vsnprintf(s, SPRINTF_UNLIMITED, fmt, args);
}

int sprintf(char *s, const char *fmt, ...)
{
	va_list args;
	int err;

	va_start(args, fmt);
	err = vsprintf(s, fmt, args);
	va_end(args);

	return err;
}

int snprintf(char *s, size_t size, const char *fmt, ...)
{
	va_list args;
	int err;

	va_start(args, fmt);
	err = vsnprintf(s, size, fmt, args);
	va_end(args);

	return err;
}

