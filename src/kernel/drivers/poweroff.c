// See LICENSE for license details.

#include "stdlib.h"

#include "drivers/poweroff.h"

static void default_poweroff(int status)
{
	__asm__ __volatile__("ebreak");
	while (1)
	{
		__asm__ __volatile__("" : : : "memory");
	}
}

poweroff_device_t poweroff_none = {
		.init = NULL,
		.poweroff = default_poweroff,
};

poweroff_device_t *poweroff_dev = &poweroff_none;

void register_poweroff(poweroff_device_t *dev)
{
	poweroff_dev = dev;
	if(dev->init)
	{
		dev->init();
	}
}

void exit(int status)
{
	poweroff_dev->poweroff(status);
	__asm__ __volatile__("1: j 1b");
	__builtin_unreachable();
}
