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
#include "drivers/plic.h"
#include "drivers/clint.h"
#include "drivers/poweroff.h"
#include "drivers/console.h"

char *default_program = "console";

auxval_t __auxv[] = {
		{UART0_CLOCK_FREQ,    32000000},
		{UART0_BAUD_RATE,     115200},
		{CEP_UART0_CTRL_ADDR, 0x10013000},
		{CEP_POWEROFF_CTRL_ADDR,  0x10000000},
		{0,                   0}
};

void arch_setup()
{
	register_console(&console_cep_uart);
	register_poweroff(&cep_poweroff);
	register_clint(&cep_clint);
	register_plic(&cep_plic);

	// activation des compteurs de performances
	csr_write(mcounteren, -1);
	csr_write(scounteren, -1);
}
