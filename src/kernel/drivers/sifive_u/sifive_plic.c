#include "stdint.h"

#include "drivers/plic.h"

#define SIFIVE_PLIC_BASE          0xc000000
#define SIFIVE_PLIC_ENABLE_OFFSET 0x2000
#define SIFIVE_PLIC_TARGET_OFFSET 0x200000
#define SIFIVE_PLIC_SOURCE_OFFSET 0x0

#define SIFIVE_PLIC_ENABLE        (SIFIVE_PLIC_BASE + SIFIVE_PLIC_ENABLE_OFFSET)
#define SIFIVE_PLIC_TARGET        (SIFIVE_PLIC_BASE + SIFIVE_PLIC_TARGET_OFFSET)
#define SIFIVE_PLIC_SOURCE        (SIFIVE_PLIC_BASE + SIFIVE_PLIC_SOURCE_OFFSET)

#define CEP_UART0_IRQ             1

static void sifive_plic_init()
{
	// target 0 (cpu M mode) enable -> everything
	uint64_t* plic_addr = (uint64_t*) SIFIVE_PLIC_ENABLE;
	*plic_addr = -1;
	plic_addr++;
	*plic_addr = -1;

	// target 0 priority threshold (all interruption from source with prio > 0 will be raised
	*((uint64_t*) SIFIVE_PLIC_TARGET) = 0;

	*((uint32_t*) (SIFIVE_PLIC_SOURCE + CEP_UART0_IRQ * 0x4)) = 1; // source 1 priority (uart interrupt)

	// target 1 (cpu S mode) enable -> everything
	plic_addr = (uint64_t*) (SIFIVE_PLIC_ENABLE + 0x80);
	*plic_addr = -1;
	plic_addr++;
	*plic_addr = -1;

	// target 1 priority threshold (all interruption from source with prio > 0 will be raised
	*((uint64_t*) (SIFIVE_PLIC_TARGET + 0x1000)) = 0;
}

plic_device_t sifive_plic = {
	sifive_plic_init,
};
