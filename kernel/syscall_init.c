/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#include "syscall_init.h"

extern void syscall_handler(void);

void syscall_init(void)
{
#if defined(ARCH_x86_64)
	uint32_t efer_low, efer_high;
	__asm__ volatile ("rdmsr":"=a" (efer_low), "=d"(efer_high):"c"(0xC0000080));

	efer_low |= 1;
	__asm__ volatile ("wrmsr"::"a" (efer_low), "d"(efer_high), "c"(0xC0000080));

	extern void syscall_handler(void);
	uint64_t handler_addr = (uint64_t) syscall_handler;
	__asm__ volatile ("wrmsr"::"a" ((uint32_t) handler_addr), "d"((uint32_t) (handler_addr >> 32)),
		"c"((uint32_t) 0xC0000082));

	uint32_t star_high = (0x08 << 16) | (0x1B);
	__asm__ volatile ("wrmsr"::"a" (0), "d"(star_high), "c"(0xC0000081));

	__asm__ volatile ("wrmsr"::"a" (0x200), "d"(0), "c"(0xC0000084));
#endif

#if defined(ARCH_riscv64)
	/* `mtvec` (Machine Mode) or `stvec` (Supervisor Mode) */
	__asm__ volatile ("csrw stvec, %0"::"r" (syscall_handler));
#endif
}
