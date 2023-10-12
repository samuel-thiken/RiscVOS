/*
 * Copyright (C) 2005 -- Simon Nieuviarts
 * Copyright (C) 2012 -- Damien Dejean <dam.dejean@gmail.com>
 *
 * Kernel memory allocator.
 */

#include "mem.h"
#include "stddef.h"
#include "stdint.h"
#include "riscv.h"
#include "process.h"
#include "assert.h"
#include "stdlib.h"

typedef struct frame {
	struct frame* next_frame;
} frame;
#define FRAME_SIZE 4096

void* KERNEL_TABLE_ADDR;

/* Heap boundaries */
extern char _heap_start[];
extern char _heap_end[];

extern char _free_memory_start[];
extern char _memory_end[];

frame* first_available_frame = NULL;

static char *curptr = _heap_start;

void create_kernel_memory() {
	// Création de la table du kernel
	memory_tables_t kernel_memory = get_frame();

	KERNEL_TABLE_ADDR = kernel_memory;

	kernel_memory[0] = (table_entry_t){
		.fields = {
			.valid = 1,
			.write = 1,
			.read = 1,
			.execute = 1,
            .global = 0,
            .accessed = 0,
            .dirty = 0,
            .rsw = 0,
			.ppn0 = 0,
			.ppn1 = 0,
			.ppn2 = 0}};

	// Mémoire vidéo
	kernel_memory[2] = (table_entry_t){
		.fields = {
			.valid = 1,
			.write = 1,
			.read = 1,
			.execute = 1,
            .global = 0,
            .accessed = 0,
            .dirty = 0,
            .rsw = 0,
			.ppn0 = 0,
			.ppn1 = 0,
			.ppn2 = 2}};
	change_satp(kernel_memory);
}

void init_frames() {
	first_available_frame = (frame *) ((void *) _free_memory_start) + 5 * FRAME_SIZE;
	frame* next_frame = first_available_frame;
	int nb_frames = 0;
	while ((void *) (next_frame + FRAME_SIZE) < (void *) _memory_end) {
		nb_frames++;
		next_frame->next_frame = next_frame + FRAME_SIZE;
		next_frame = next_frame + FRAME_SIZE;
	}

	next_frame->next_frame = NULL;
	printf("%d frames\n", nb_frames);
}

void* get_frame() {
	frame* allocated_frame = first_available_frame;
	if (allocated_frame == NULL) return NULL;
	first_available_frame = first_available_frame->next_frame;

	return (void*) allocated_frame;

	// void* addr = memalign(4096, 4096);
	// if (addr == KERNEL_TABLE_ADDR) addr = memalign(4096, 4096);
	// return addr;
}
void release_frame(void *frame_to_release) {
	frame* released_frame = (frame*) frame_to_release;

	released_frame->next_frame = first_available_frame;
	first_available_frame = released_frame;
}

void change_satp(memory_tables_t tables) {
	csr_write(satp, get_satp(tables));
}
uint64_t get_satp(memory_tables_t tables) {
	return (uint64_t) (8ul << 60 | (uint64_t) (((uint64_t) tables) >> 12));
}

/* Trivial sbrk implementation */
void *sbrk(ptrdiff_t diff)
{
	char *s = curptr;
	char *c = s + diff;
	if ((c < curptr) || (c > _heap_end)) return ((void*)(-1));
	curptr = c;
	return s;
}

