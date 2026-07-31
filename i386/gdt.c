/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#include "gdt.h"

static struct gdt_entry gdt[3];
static struct gdt_ptr gp;

static void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
	gdt[num].base_low = (base & 0xFFFF);
	gdt[num].base_middle = (base >> 16) & 0xFF;
	gdt[num].base_high = (base >> 24) & 0xFF;

	gdt[num].limit_low = (limit & 0xFFFF);
	gdt[num].granularity = ((limit >> 16) & 0x0F);

	gdt[num].granularity |= (gran & 0xF0);
	gdt[num].access = access;
}

void gdt_init(void)
{
	gp.limit = (sizeof(struct gdt_entry) * 3) - 1;
	gp.base = (uintptr_t) & gdt;

	gdt_set_gate(0, 0, 0, 0, 0);	/* Null descriptor */
	gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);	/* Code segment */
	gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);	/* Data segment */

	__asm__ volatile ("lgdt %0"::"m" (gp));

	__asm__ volatile ("ljmp $0x08, $.reload_segments\n"
		".reload_segments:\n"
		"mov $0x10, %ax\n"
		"mov %ax, %ds\n" "mov %ax, %es\n" "mov %ax, %fs\n" "mov %ax, %gs\n" "mov %ax, %ss\n");
}
