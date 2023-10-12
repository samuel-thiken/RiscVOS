// See LICENSE for license details.

#include "drivers/console.h"
#include "stdlib.h"

static int default_putchar(int ch)
{
	__asm__ __volatile__("ebreak");
	return 0;
}

static int default_getchar()
{
	__asm__ __volatile__("ebreak");
	return 0;
}

console_device_t console_none = {
		.init = NULL,
		.getchar = default_getchar,
		.putchar = default_putchar
};

console_device_t *console_dev = &console_none;

void register_console(console_device_t *dev)
{
	console_dev = dev;
	if(dev->init)
	{
		dev->init();
	}
}
