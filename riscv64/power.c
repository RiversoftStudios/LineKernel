/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#include "power.h"

#ifdef CONFIG_VIRT_MACHINE
#define SYSCON_ADDR 0x100000
#endif

void poweroff(void)
{
	*(uint32_t *) SYSCON_ADDR = 0x5555;
}

void reboot(void)
{
	*(uint32_t *) SYSCON_ADDR = 0x7777;
}

void halt(void)
{
	while (1) {
		__asm__ volatile ("wfi");
	}
}
