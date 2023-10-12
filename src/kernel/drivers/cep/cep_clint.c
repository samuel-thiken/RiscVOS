/*
 * Projet PCSEA RISC-V
 *
 * Benoît Wallon <benoit.wallon@grenoble-inp.org> - 2019
 * Mathieu Barbe <mathieu@kolabnow.com> - 2019 - 2020
 *
 * See license for license details.
 */

#include "drivers/clint.h"

clint_device_t cep_clint = {
		.clk_freq = 10000000,
		.base_addr = 0x2000000,
		.supervisor_clint_available = true,
		.user_clint_available = true,
};
