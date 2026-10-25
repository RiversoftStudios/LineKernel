/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#include "power.h"
#include "kconfig.h"

#ifdef CONFIG_VIRT_MACHINE
#define SYSCON_ADDR 0x100000
#endif

/* TODO: Some of these values are specific to `virt`. */

ATTRIBUTE_NORETURN void halt(void)
{
	while (1) {
		__asm__ volatile ("wfi");
	}
}

ATTRIBUTE_NORETURN void poweroff(void)
{
	*(uint32_t *) SYSCON_ADDR = 0x5555;

	halt();
}

ATTRIBUTE_NORETURN void reboot(void)
{
	*(uint32_t *) SYSCON_ADDR = 0x7777;

	halt();
}
