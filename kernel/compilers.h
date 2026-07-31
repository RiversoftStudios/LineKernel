/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#ifndef __COMPILERS_H__
#define __COMPILERS_H__

/* For now we use standard headers for stdint. This will probably stay like this though. */
#include <stdint.h>

#if defined(__GNUC__) || defined(__clang__)
	#define ATTRIBUTE_NORETURN __attribute__((noreturn))
#elif defined(_MSC_VER)
	#define ATTRIBUTE_NORETURN __declspec(noreturn)
#elif __STDC_VERSION__ >= 201112L
	#define ATTRIBUTE_NORETURN _Noreturn
#else
	#define ATTRIBUTE_NORETURN
#endif

#if defined(__x86_64__) || defined(__amd64__)
	#define CPU_BITS 64
#elif defined(__i386__) || defined(_M_IX86)
	#define CPU_BITS 32
#elif defined(__riscv)
	#if __riscv_xlen == 64
		#define CPU_BITS 64
	#elif __riscv_xlen == 32
		#define ARCH_BITS 32
	#else
		#error "Unknown RISC-V bit width"
	#endif
#else
	#error "Unsupported architecture"
#endif

#endif
