/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#include "power.h"
#include "bios_tools.h"
#include "LineRenderer.h"

void halt(void)
{
	__asm__ volatile ("cli");

	while (1) {
		__asm__ volatile ("hlt");
	}
}

void reboot(void)
{
	uint8_t good = 0x02;

	while (good & 0x02)
		good = inb(0x64);
	outb(0x64, 0xFE);
	halt();
}

void poweroff(void)
{
	/* TODO: ACPI */
	terminal_cls();
	terminal_writestring("It is now safe to power off your computer.");
	halt();
}
