/*
 * Projet PCSEA RISC-V
 *
 * Benoît Wallon <benoit.wallon@grenoble-inp.org> - 2019
 * Mathieu Barbe <mathieu@kolabnow.com> - 2019 - 2020
 *
 * See license for license details.
 */

#include "stdint.h"
#include "drivers/auxval.h"

#include "drivers/poweroff.h"

#define CEP_VALUE_EXIT  0x3333

static volatile uint32_t *test;

static void cep_test_init()
{
	test = (uint32_t *) (void *) getauxval(CEP_POWEROFF_CTRL_ADDR);
}

static void cep_test_poweroff(int status)
{
	*test = (status << 16) | CEP_VALUE_EXIT;
	while (1);
}

poweroff_device_t cep_poweroff = {
		cep_test_init,
		cep_test_poweroff
};
