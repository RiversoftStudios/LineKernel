/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#ifndef __LINEDRIVE_H__
#define __LINEDRIVE_H__

#include "kconfig.h"
#include <stdint.h>

#ifdef CONFIG_FLOPPY
#include "floppy.h"
#endif

enum drivesformat {
	nodrive,
	fdrive1,
};

typedef enum drivesformat drivesformat_t;

#ifdef CONFIG_DRIVE
int drive_init(drivesformat_t drive);
int read_drive_sector(drivesformat_t drive, uint32_t lba, uint32_t * out_buf);
int write_drive_sector(drivesformat_t drive, uint32_t lba, const uint32_t * in_buf);
const char* drive_name(drivesformat_t drive);
#endif

#endif
