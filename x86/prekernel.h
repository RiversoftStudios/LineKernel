/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#ifndef __PREKERNEL_H__
#define __PREKERNEL_H__

#include "LineKernel.h"
#include "LineRenderer.h"
#include <stdint.h>
#include "multiboot.h"
#include "str.h"
void multiboot_pre_main(uint32_t magic, uint32_t mbi_address);

#define MULTIBOOT_CHECK_FLAG(flag, bit) ((flag) & (1 << (bit)))

#endif
