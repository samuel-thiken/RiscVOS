#include "test1.h"
#include "sysapi.h"

int main(void *arg)
{
        printf(" 5");
        assert((long int) arg == DUMMY_VAL + 1);
        return 4;
}
