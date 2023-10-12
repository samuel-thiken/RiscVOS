#include "tests.h"
#include "stdio.h"
#include "assert.h"
#include "process.h"

int no_run(void *arg)
{
    (void)arg;
    assert(0);
    return 1;
}

int waiter(void *arg)
{
    int pid = (int)(long int)arg;
    assert(kill(pid) == 0);
    assert(waitpid(pid, 0) < 0);
    return 1;
}

int test5(void *arg)
{
    int pid1, pid2;
    int r;

    (void)arg;

    // Le processus 0 et la priorite 0 sont des parametres invalides
    assert(kill(0) < 0);
    assert(chprio(getpid(), 0) < 0);
    assert(getprio(getpid()) == 128);
    pid1 = start_in_kernel(no_run, 4000, 64, "no_run", 0);
    assert(pid1 > 0);
    assert(kill(pid1) == 0);
    assert(kill(pid1) < 0);        // pas de kill de zombie
    assert(chprio(pid1, 128) < 0); // changer la priorite d'un zombie
    assert(chprio(pid1, 64) < 0);  // changer la priorite d'un zombie
    assert(waitpid(pid1, 0) == pid1);
    assert(waitpid(pid1, 0) < 0);
    pid1 = start_in_kernel(no_run, 4000, 64, "no_run", 0);
    assert(pid1 > 0);
    pid2 = start_in_kernel(waiter, 4000, 65, "waiter", (void *)(long int)pid1);
    assert(pid2 > 0);
    assert(waitpid(pid2, &r) == pid2);
    assert(r == 1);
    assert(waitpid(pid1, &r) == pid1);
    assert(r == 0);
    printf("ok.\n");
    printf("\t\t\t\t\t\t\t[OK]\n");
    return 0;
}
