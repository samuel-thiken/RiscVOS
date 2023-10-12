/*
 * Projet PCSEA RISC-V
 *
 * Benoît Wallon <benoit.wallon@grenoble-inp.org> - 2019
 * Mathieu Barbe <mathieu@kolabnow.com> - 2019
 *
 * See license for license details.
 */

#include "stdint.h"
#include "string.h"
#include "riscv.h"

#include "drivers/auxval.h"
#include "drivers/clint.h"
#include "drivers/poweroff.h"
#include "drivers/console.h"

extern uint64_t tohost;
extern uint64_t fromhost;

char * default_program = "autotest";

auxval_t __auxv[] = {
		{RISCV_HTIF_BASE_ADDR, 0},
		{0,                    0}
};

void arch_setup()
{
	__auxv[0].val = (uintptr_t) (&tohost < &fromhost ? &tohost : &fromhost);
	register_console(&console_htif);
	register_poweroff(&poweroff_htif);
	register_clint(&spike_clint);

	// activation des compteurs de performances
	csr_write(mcounteren, -1);
	csr_write(scounteren, -1);
}
