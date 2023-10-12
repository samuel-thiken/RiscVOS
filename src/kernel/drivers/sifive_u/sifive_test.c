// See LICENSE for license details.

#include "stdint.h"
#include "drivers/auxval.h"
#include "drivers/poweroff.h"

#define SIFIVE_TEST_EXIT  0x5555

static volatile uint32_t *test;

static void sifive_test_init()
{
	test = (uint32_t *)(void *)getauxval(SIFIVE_TEST_CTRL_ADDR);
}

static void sifive_test_poweroff(int status)
{
	*test = (status << 16) | SIFIVE_TEST_EXIT;
	while (1);
}

poweroff_device_t poweroff_sifive_test = {
	sifive_test_init,
	sifive_test_poweroff
};
