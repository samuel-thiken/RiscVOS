/*
 * Projet PCSEA RISC-V
 *
 * Benoît Wallon <benoit.wallon@grenoble-inp.org> - 2019
 * Mathieu Barbe <mathieu@kolabnow.com> - 2019 - 2020
 *
 * See license for license details.
 */

#include "stdint.h"

#include "drivers/plic.h"


#define CEP_PLIC_BASE          0xc000000
#define CEP_PLIC_ENABLE_OFFSET 0x2000
#define CEP_PLIC_TARGET_OFFSET 0x200000
#define CEP_PLIC_SOURCE_OFFSET 0x0

#define CEP_PLIC_ENABLE        (CEP_PLIC_BASE + CEP_PLIC_ENABLE_OFFSET)
#define CEP_PLIC_TARGET        (CEP_PLIC_BASE + CEP_PLIC_TARGET_OFFSET)
#define CEP_PLIC_SOURCE        (CEP_PLIC_BASE + CEP_PLIC_SOURCE_OFFSET)

#define CEP_UART0_IRQ             1

static void cep_plic_init()
{
	// target 0 (cpu M mode) enable -> everything
	uint32_t *plic_addr = (uint32_t *)CEP_PLIC_ENABLE;
	*plic_addr = -1;
	plic_addr++;
	*plic_addr = -1;

	// target 0 priority threshold (all interruption from source with prio > 0 will be raised
	*((uint32_t *)CEP_PLIC_TARGET) = 0;

	*((uint32_t *)(CEP_PLIC_SOURCE + CEP_UART0_IRQ * 0x4)) = 1; // source 1 priority (uart interrupt)

	// target 1 (cpu S mode) enable -> everything
	plic_addr = (uint32_t *)(CEP_PLIC_ENABLE + 0x80);
	*plic_addr = -1;
	plic_addr++;
	*plic_addr = -1;

	// target 1 priority threshold (all interruption from source with prio > 0 will be raised
	*((uint32_t *)(CEP_PLIC_TARGET + 0x1000)) = 0;
}

plic_device_t cep_plic = {
		cep_plic_init,
};
