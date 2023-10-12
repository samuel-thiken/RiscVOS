/*
 * Ensimag - Projet système
 * Copyright (C) 2013 - Damien Dejean <dam.dejean@gmail.com>
 */

#include "sysapi.h"

#define TESTS_NUMBER 20

const char *tests[TESTS_NUMBER] = {
		"test0",
		"test1",
		"test2",
		"test3",
		"test4",
        "test5",
        "test7",
        "test8",
        "test10",
        "test11",
        "test12",
        "test13",
        "test14",
        "test15",
        "test16",
        "test17",
        "test19",
        "test20",
        "test21",
        "test22",
};

/*
        "test6",
        "test9",
        "test14",
        "test15",
        "test16",
        "test17",
        "test18",
        "test19",
        "test20",
        "test21",
        "test22",
};
*/

int main(void)
{
	int i;
	int pid;
	long int ret;
	printf("Auto test with %d tests\n", TESTS_NUMBER);
	for (i = 0; i < TESTS_NUMBER; i++) {
		printf("Test %s : ", tests[i]);
		pid = start(tests[i], 4000, 128, NULL);
		waitpid(pid, &ret);
		assert(ret == 0);
	}
	printf("Auto test done.\n");
}

