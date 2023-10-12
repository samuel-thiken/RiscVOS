/*
 * Projet PCSEA RISC-V
 * Mathieu Barbe <mathieu@kolabnow.com> - 2019
 * Frédéric Pétrot <frederic.petrot@univ-grenoble-alpes.fr> - 2022
 * See license for license details.
 */

#pragma once
#include "stdint.h"
/* Context switching is done by a function call:
 * the riscv abi says that the only warranty we have is that the
 * registers sp and s0 to s11 are preserved accros function calls.
 * The rest has to be considered scratch when returning from the calls,
 * thus only these registers need be saved in the context.
 * Note: a few more registers will need to be saved when introducing
 * supervisor/user processes.
 */

typedef struct context {
	uint64_t sp;
	uint64_t ra;
	uint64_t s0;
	uint64_t s1;
	uint64_t s2;
	uint64_t s3;
	uint64_t s4;
	uint64_t s5;
	uint64_t s6;
	uint64_t s7;
	uint64_t s8;
	uint64_t s9;
	uint64_t s10;
	uint64_t s11;
} context_t;

/*
 * Save the current context on the stack and restore a previously saved context
 * current	: data structure in which the current context will be saved
 * future	: data structure holding the context of the future process to execute
 */
void context_switch(context_t *current, context_t *future);
