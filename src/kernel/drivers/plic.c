// See LICENSE for license details.
#include "stdlib.h"

#include "drivers/plic.h"

plic_device_t plic_none = {
		.init = NULL,
};

plic_device_t *plic_dev = &plic_none;

void register_plic(plic_device_t *dev)
{
	plic_dev = dev;
	if (dev->init) {
		dev->init();
	}
}
