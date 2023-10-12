#pragma once

#include "stdint.h"
#include "liste.h"

#define TICK_FREQUENCY 1000

// Liste qui fait le lien entre une fonction et sa fréquence d'appel
extern list_t *freq_call_func;

// Ajoute un élément dans la liste précédente
extern void schedule(void (*function)(), uint64_t freq);

/*
 * Prototypes
 */
void handle_mtimer_interrupt();
void set_machine_timer_interrupt(uint64_t delta_ms);
void handle_stimer_interrupt();

void init_clock();

void clock_settings(unsigned long *quartz, unsigned long *ticks);

unsigned long current_clock();
