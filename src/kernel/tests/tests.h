#ifndef _TESTS_H_
#define _TESTS_H_

#include "stdint.h"

#define DUMMY_VAL 0xDEADCAFE

/*
 * Run kernel test suite
 */
int kernel_tests(void *arg);

/*
 * Start a simple process
 */
int test0(void* arg);

/*
 * Start a process with a parameter
 * End normally
 * Father wait for his child
 */
int test1(void* arg);

/*
 * Test kill and exit
 */
int test2(void* arg);

/*
 * Prioities and chprio
 */
int test3(void* arg);

/*
 * shared time and active waiting
 */
int test4(void* arg);

/*
 * Robustness of the scheduler
 */
int test5(void* arg);


int test7(void* arg);


/*
 * Creation de processus se suicidant en boucle.
 * Test de la vitesse de creation de processus.
 */
int test8(void* arg);

/*
 * Semaphores or message queues test
 */
int test10(void* arg);

int test12_msg(void* arg);

int test14_msg(void* arg);

int test15_msg(void *arg);

#endif /* _TESTS_H_ */
