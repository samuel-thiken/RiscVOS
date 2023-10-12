#include <stddef.h>
#include <stdio.h>

#include "tests.h"
#include "../userspace_apps.h"
#include "../process.h"



int kernel_tests(void *arg)
{
	int rc = 0;

	puts("Run kernel test suite:");

	int test_0;
	printf("\nTest 0: Mode User\n");
	const struct uapps *test0_app = find_app("test0");
	int pid_0 = start(test0_app->start, 512, 11, "test0", NULL);
	waitpid(pid_0, &test_0);
	if (test_0)
		rc = 1;

	int test_1;
	printf("\nTest 1: Sous processus\n");
	int pid_1 = start_in_kernel(test1, 512, 10, "test1", NULL);
	waitpid(pid_1, &test_1);
	if (test_1)
		rc = 1;

	int test_2;
	printf("\nTest 2: Kill et Exit\n");
	int pid_2 = start_in_kernel(test2, 512, 100, "test2", NULL);
	waitpid(pid_2, &test_2);
	if (test_2)
		rc = 1;

	int test_3;
	printf("\nTest 3: Changement de priorité\n");
	int pid_3 = start_in_kernel(test3, 512, 128, "test3", NULL);
	waitpid(pid_3, &test_3);
	if (test_3)
		rc = 1;

	int test_4;
	printf("\nTest 4: Ordonnancement\n");
	int pid_4 = start_in_kernel(test4, 512, 128, "test4", NULL);
	waitpid(pid_4, &test_4);
	if (test_4)
		rc = 1;

	int test_5;
	printf("\nTest 5: Attente de processus\n");
	int pid_5 = start_in_kernel(test5, 512, 128, "test5", NULL);
	waitpid(pid_5, &test_5);
	if (test_5)
		rc = 1;

	// int test_8;
	// printf("\nTest 8:\n");
	// int pid_8 = start_in_kernel(test8, 512, 128, "test8", NULL);
	// waitpid(pid_8, &test_8);
	// if (test_8)
	// 	rc = 1;

	int test_10;
	printf("\nTest 10: File de message\n");
	int pid_10 = start_in_kernel(test10, 512, 128, "test10", NULL);
	waitpid(pid_10, &test_10);
	if (test_10)
		rc = 1;

	int test_12;
	printf("\nTest 12: File de message\n");
	int pid_12 = start_in_kernel(test12_msg, 512, 128, "test12_msg", NULL);
	waitpid(pid_12, &test_12);
	if (test_12)
		rc = 1;
	
	int test_14;
	printf("\nTest 14: File de message\n");
	int pid_14 = start_in_kernel(test14_msg, 512, 128, "test14_msg", NULL);
	waitpid(pid_14, &test_14);
	if (test_14)
		rc = 1;

	int test_15;
	printf("\nTest 15: File de message\n");
	int pid_15 = start_in_kernel(test15_msg, 512, 128, "test15_msg", NULL);
	waitpid(pid_15, &test_15);
	if (test_15)
		rc = 1;

	/*
	 * Dans un second temps (quand vous aurez la création de task/processus), les tests devront
	 * être exécutés dans un processus dédié.
	 * Comme par exemple:
	 * int test_rc;
	 * int pid = sched_kstart_in_kernel(test0, 10, "Test 0", 0);
	 * sched_waitpid(pid, &test_rc);
	 * if (test_rc)
	 * 	rc = 1;
	 */

	return rc;
}
