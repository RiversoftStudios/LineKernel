/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

/* TODO: Make this more portable(?) */

#include "timer.h"
#include "idt.h"
#include "bios_tools.h"

static volatile uint32_t timer_ticks = 0;
static uint32_t timer_frequency = 1000;

static void timer_callback(struct regs* r)
{
	(void)r;
	timer_ticks++;
}

void timer_init(uint32_t frequency)
{
	timer_frequency = frequency;
	/* Install callback for IRQ 0 (timer) */
	irq_install_handler(0, timer_callback);

	/* PIT divisor */
	uint32_t divisor = 1193182 / frequency;

	/* Send command byte (Channel 0, access lobyte/hibyte, Mode 3, 16-bit binary) */
	outb(0x43, 0x36);

	/* Send divisor byte by byte */
	outb(0x40, (uint8_t) (divisor & 0xFF));
	outb(0x40, (uint8_t) ((divisor >> 8) & 0xFF));
}

uint32_t timer_get_ticks(void)
{
	return timer_ticks;
}

void sleep_ms(uint32_t ms)
{
	uint32_t start = timer_ticks;

	while (timer_ticks - start < ms) {
		/* Halt CPU while waiting for next interrupt to conserve power */
		__asm__ volatile ("hlt");
	}
}
