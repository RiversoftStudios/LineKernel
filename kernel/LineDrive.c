/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#include "LineDrive.h"

#ifdef CONFIG_DRIVE
int drive_init(drivesformat_t drive)
{
	switch (drive) {
#ifdef CONFIG_FLOPPY
	case fdrive1:
		return floppy_init();
#endif
	default:
		return -1;
	}
}

/* How many bits should out_buf be? */
int read_drive_sector(drivesformat_t drive, uint32_t lba, uint32_t* out_buf)
{
	switch (drive) {
#ifdef CONFIG_FLOPPY
	case fdrive1:
		return floppy_read_sector(lba, (uint8_t *) out_buf);
#endif
	default:
		return -1;
	}
}

int write_drive_sector(drivesformat_t drive, uint32_t lba, const uint32_t* in_buf)
{
	switch (drive) {
#ifdef CONFIG_FLOPPY
	case fdrive1:
		return floppy_write_sector(lba, (uint8_t *) in_buf);
#endif
	default:
		return -1;
	}
}

const char* drive_name(drivesformat_t drive)
{
	switch (drive) {
	case fdrive1:
		return "fdrive1";
	default:
		return "(none)";
	}
}
#endif
