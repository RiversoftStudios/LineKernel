/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

/* TODO: Test this more. */

#include "LineMemory.h"

/* Yes I know, it's not that good. But it works. */

#define ALIGNMENT 16
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))

typedef struct block_header {
	size_t size;				/* Total size of the block including header */
	uint8_t is_free;			/* 1 if free, 0 if allocated */
	struct block_header* next;	/* Next free block in explicit list */
	struct block_header* prev;	/* Prev free block in explicit list */
} block_header_t;

/* Defined in linker script */
extern uint8_t __free_ram[];
extern uint8_t __free_ram_end[];

static block_header_t* free_list_head = NULL;

void heap_init(void)
{
	uintptr_t start = ALIGN((uintptr_t) __free_ram);
	uintptr_t end = ((uintptr_t) __free_ram_end) & ~(ALIGNMENT - 1);

	if (start >= end)
		return;

	size_t total_size = end - start;

	free_list_head = (block_header_t *) start;
	free_list_head->size = total_size;
	free_list_head->is_free = 1;
	free_list_head->next = NULL;
	free_list_head->prev = NULL;
}

static void remove_from_free_list(block_header_t* block)
{
	if (block->prev) {
		block->prev->next = block->next;
	} else {
		free_list_head = block->next;
	}
	if (block->next) {
		block->next->prev = block->prev;
	}
	block->next = NULL;
	block->prev = NULL;
}

static void add_to_free_list(block_header_t* block)
{
	block->is_free = 1;
	block->next = free_list_head;
	block->prev = NULL;
	if (free_list_head) {
		free_list_head->prev = block;
	}
	free_list_head = block;
}

void* kmalloc(size_t size)
{
	if (size == 0)
		return NULL;

	size_t total_requested = ALIGN(size + sizeof(block_header_t));

	block_header_t* curr = free_list_head;

	while (curr) {
		if (curr->size >= total_requested) {
			remove_from_free_list(curr);
			curr->is_free = 0;

			size_t remaining = curr->size - total_requested;

			if (remaining >= sizeof(block_header_t) + ALIGNMENT) {
				curr->size = total_requested;

				block_header_t* next_block =
					(block_header_t *) ((uintptr_t) curr + total_requested);
				next_block->size = remaining;
				add_to_free_list(next_block);
			}

			return (void *)((uintptr_t) curr + sizeof(block_header_t));
		}
		curr = curr->next;
	}

	return NULL;				/* No memory left */
}

static void coalesce(block_header_t* block)
{
	uintptr_t heap_end = (uintptr_t) __free_ram_end;
	uintptr_t next_addr = (uintptr_t) block + block->size;

	if (next_addr < heap_end) {
		block_header_t* next_block = (block_header_t *) next_addr;

		if (next_block->is_free == 1) {
			remove_from_free_list(next_block);
			block->size += next_block->size;
		}
	}
}

void kfree(void* ptr)
{
	if (!ptr)
		return;

	block_header_t* block = (block_header_t *) ((uintptr_t) ptr - sizeof(block_header_t));

	if (block->is_free)
		return;

	add_to_free_list(block);
	coalesce(block);
}
