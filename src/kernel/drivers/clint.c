// See LICENSE for license details.

#include "drivers/clint.h"

clint_device_t clint_none = {
		.base_addr = 0,
		.clk_freq = 0,
};

clint_device_t *clint_dev = &clint_none;

void register_clint(clint_device_t *dev)
{
	clint_dev = dev;
}
