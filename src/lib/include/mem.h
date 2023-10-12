/*
 * Copyright (C) 2005 Simon Nieuviarts
 *
 * Memory allocator for kernel data.
 */
#ifndef __MEM_H__
#define __MEM_H__

#include <stdint.h>

extern void* KERNEL_TABLE_ADDR;

void *mem_alloc(unsigned long length);
void mem_free(void *zone, unsigned long length);

void init_frames();
void *get_frame();
void release_frame(void *);
void create_kernel_memory();

typedef union {
	struct {
		uint64_t page_offset:12;
		uint64_t vpn0:9;
		uint64_t vpn1:9;
		uint64_t vpn2:9;
	} fields;
	uint64_t bits;
} virtual_address_t;

typedef union {
	struct {
		uint64_t valid:1;
		uint64_t read:1;
		uint64_t write:1;
		uint64_t execute:1;
		uint64_t user:1;
		uint64_t global:1;
		uint64_t accessed:1;
		uint64_t dirty:1;
		uint64_t rsw:2;
		uint64_t ppn0:9;
		uint64_t ppn1:9;
		uint64_t ppn2:26;
	} fields;
	struct {
		uint64_t valid:1;
		uint64_t read:1;
		uint64_t write:1;
		uint64_t execute:1;
		uint64_t user:1;
		uint64_t global:1;
		uint64_t accessed:1;
		uint64_t dirty:1;
		uint64_t rsw:2;
		uint64_t ppn:44;
	} fields_merged;
} table_entry_t;

typedef table_entry_t* memory_tables_t;

typedef void* frame_t;

void change_satp(memory_tables_t tables);
uint64_t get_satp(memory_tables_t tables);

#endif
