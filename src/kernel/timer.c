#include <stdlib.h>
#include "riscv.h"
#include "stdio.h"

#include "drivers/clint.h"
#include "timer.h"
#include "drivers/sifive_u/sifive_clint.c"
#include "liste.h"
#include "mem.h"

static int timer = 0;
list_t *freq_call_func = NULL;

typedef struct process_timer_t
{
	void (*function)();
	uint64_t freq;
} process_timer_t;

/*
 * Set machine timer
 *
 * Cette fonction configure la prochaine interruption timer machine delta ms dans le futur.
 *
 *@param delta_ms : réglage de la prochaine interruption à timer machine  cur + delta ms.
 */
void set_machine_timer_interrupt(uint64_t delta_ms)
{
	/*
	 * Configuration du clint
	 * L'adresse et la fréquence du clint est dépendante de la plateforme,
	 * La variable globale clint_dev de type clint_device_t nous renseigne sur ces paramettres:
	 * - clint_dev->clk_freq indique la fréquence d'incrémentations du registre time.
	 * - clint_dev->base_addr indique l'adresse où est mapé la base des registres du clint.
	 *
	 * Les macros get_mtime() et set_mtimecmp(x) de "drivers/clint.h" sont données pour lire et écrire 
	 * les registres de ce composant.
	 */
	set_mtimecmp(get_mtime() + delta_ms * (clint_dev->clk_freq / 1000));
}

void handle_mtimer_interrupt()
{
	// printf("MTic ");
	set_stimecmp(get_stime() + TICK_FREQUENCY * (clint_dev->clk_freq / 1000));
	csr_clear(mie, MIP_MTIP); // on désactive le timer machine
	csr_set(mip, MIP_STIP);	  // on trigger le timer interupt du supervisor
}

void handle_stimer_interrupt()
{
	timer++;
	// printf("STic ");
	set_stimecmp(get_stime() + TICK_FREQUENCY * (clint_dev->clk_freq / 1000));

	cellule_t *next = freq_call_func->head;
	while (next != NULL)
	{
		if ((TICK_FREQUENCY * timer) % ((process_timer_t *)(next->val))->freq == 0)
		{
			// printf("timer = %d\n", timer);
			(*((process_timer_t *)(next->val))->function)();
		}
		next = next->next;
	}
}

void init_clock()
{
	set_machine_timer_interrupt(1000);
	csr_set(mstatus, MSTATUS_MIE);
	csr_set(mie, MIP_MTIP); // on active les interupts machine
	csr_set(sie, MIP_STIP); // on active les interupts du supervisor
}

void clock_settings(unsigned long *quartz, unsigned long *ticks)
{
	*quartz = clint_dev->clk_freq;
	*ticks = 1000;
}

unsigned long current_clock()
{
	return timer;
}

void schedule(void (*function)(), uint64_t freq)
{
	process_timer_t *elem = mem_alloc(sizeof(process_timer_t));
	elem->freq = freq;
	elem->function = function;
	add_queue(freq_call_func, elem);
}
