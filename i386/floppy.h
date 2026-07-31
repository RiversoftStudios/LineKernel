/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#ifndef __FLOPPY_H__
#define __FLOPPY_H__

#include <stdint.h>
#include "kconfig.h"

#ifdef CONFIG_FLOPPY
int floppy_init(void);
int floppy_read_sector(uint32_t lba, uint8_t * out_buf);
int floppy_write_sector(uint32_t lba, const uint8_t * in_buf);
int floppy_detect_fat12();
#endif

#endif
