/*
 * Ensimag - Projet système
 * Copyright (C) 2012 - Damien Dejean <dam.dejean@gmail.com>
 *
 * Defines structure to find user space programs.
 */

#include "string.h"
#include "stdio.h"
#include "assert.h"

#include "userspace_apps.h"

const struct uapps *find_app(const char *name)
{
	for (int k = 0; symbols_table[k].name != NULL; k++) {
		if (strcmp(symbols_table[k].name, name) == 0) {
			return &symbols_table[k];
		}
	}

	return NULL;
}

