/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#ifndef _LineKernel___SYSCALL_H__
#define _LineKernel___SYSCALL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <LineKernel/syscall/syscall_ids.h>

#if defined(__i386__)
static inline long syscall3(long num, long arg1, long arg2, long arg3)
{
	long ret;
	__asm__ volatile (
		"int $0x80"
		: "=a"(ret)
		: "a"(num), "b"(arg1), "c"(arg2), "d"(arg3)
		: "memory"
	);
	return ret;
}
#elif defined(__x86_64__)
static inline long syscall3(long num, long arg1, long arg2, long arg3)
{
	long ret;
	__asm__ volatile (
		"syscall"
		: "=a"(ret)
		: "a"(num), "D"(arg1), "S"(arg2), "d"(arg3)
		: "rcx", "r11", "memory"
	);
	return ret;
}
#elif defined(__riscv) && (__riscv_xlen == 64)
static inline long syscall3(long num, long arg1, long arg2, long arg3)
{
	register long a7 __asm__("a7") = num;
	register long a0 __asm__("a0") = arg1;
	register long a1 __asm__("a1") = arg2;
	register long a2 __asm__("a2") = arg3;
	__asm__ volatile (
		"ecall"
		: "+r"(a0)
		: "r"(a7), "r"(a1), "r"(a2)
		: "memory"
	);
	return a0;
}
#else
#error "This arch doesn't support syscalls!"
#endif

#ifdef __cplusplus
}
#endif

#endif
