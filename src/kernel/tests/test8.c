#include "tests.h"
#include "stdio.h"
#include "assert.h"
#include "process.h"
#include "it.h"

int suicide(void *arg)
{
    (void)arg;
    kill(getpid());
    assert(0);
    return 0;
}

int suicide_launcher(void *arg)
{
    int pid1;
    (void)arg;
    pid1 = start_in_kernel(suicide, 4000, 192, "suicide", 0);
    assert(pid1 > 0);
    return pid1;
}

int test8(void *arg)
{
    unsigned long long tsc1;
    unsigned long long tsc2;
    int i, pid, count;
    int r;

    (void)arg;
    assert(getprio(getpid()) == 128);

    /* Le petit-fils va passer zombie avant le fils mais ne pas
       etre attendu par waitpid. Un nettoyage automatique doit etre
       fait. */
    pid = start_in_kernel(suicide_launcher, 4000, 129, "suicide_launcher", 0);
    assert(pid > 0);
    assert(waitpid(pid, &r) == pid);
    assert(chprio((int)r, 192) < 0);

    count = 0;
    __asm__ __volatile__("rdcycle %0"
                         : "=r"(tsc1));
    do
    {
        for (i = 0; i < 10; i++)
        {
            pid = start_in_kernel(suicide_launcher, 4000, 200, "suicide_launcher", 0);
            assert(pid > 0);
            assert(waitpid(pid, 0) == pid);
        }
        test_it();
        count += i;
        __asm__ __volatile__("rdcycle %0"
                             : "=r"(tsc2));
    } while ((tsc2 - tsc1) < 1000000000);
    printf("%lld cycles/process.\n", (tsc2 - tsc1) / (2 * (unsigned)count));
    printf("\t\t\t\t\t\t\t[OK]\n");
    return 0;
}
