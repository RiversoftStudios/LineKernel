/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#ifndef __GDT_H__
#define __GDT_H__

#include <stdint.h>
#include "compilers.h"

struct gdt_entry {
	uint16_t limit_low;
	uint16_t base_low;
	uint8_t base_middle;
	uint8_t access;
	uint8_t granularity;
	uint8_t base_high;
} ATTRIBUTE_PACKED;

struct gdt_ptr {
	uint16_t limit;
	uint32_t base;
} ATTRIBUTE_PACKED;

void gdt_init(void);

#endif
