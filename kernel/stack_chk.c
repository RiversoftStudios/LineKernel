/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#include <stdint.h>
#include "compilers.h"
#include "panic.h"

#if UINT32_MAX == UINTPTR_MAX
#define STACK_CHK_GUARD 0xc3859a69
#else
#define STACK_CHK_GUARD 0xba2637b8b1a55f4e
#endif

uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

ATTRIBUTE_NORETURN void __stack_chk_fail(void)
{
	panic("Stack smashing detected");
}
