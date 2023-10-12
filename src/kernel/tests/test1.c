/*******************************************************************************
 * Test 1
 *
 * Demarrage de processus avec passage de parametre
 * Terminaison normale avec valeur de retour
 * Attente de terminaison (cas fils avant pere et cas pere avant fils)
 ******************************************************************************/

#include "tests.h"

#include "stdio.h"
#include "assert.h"
#include "process.h"

#define VAL 78

int dummy1(void *arg)
{
    printf("1");
    assert((long int)arg == VAL);
    return 3;
}

int dummy2(void *arg)
{
    printf(" 5");
    assert((long int)arg == VAL + 1);
    return 4;
}

int test1(void *arg)
{
    int pid1;
    int r;
    int rval;

    (void)arg;

    pid1 = start_in_kernel(dummy1, 4000, 192, "dummy1", (void *)(long int)VAL);
    assert(pid1 > 0);
    printf(" 2");
    r = waitpid(pid1, &rval);
    assert(r == pid1);
    assert(rval == 3);
    printf(" 3");
    pid1 = start_in_kernel(dummy2, 4000, 100, "dummy2", (void *)(long int)(VAL + 1));
    assert(pid1 > 0);
    printf(" 4");
    r = waitpid(pid1, &rval);
    assert(r == pid1);
    assert(rval == 4);
    printf(" 6.\n");
	printf("\t\t\t\t\t\t\t[OK]\n");
    return 0;
}