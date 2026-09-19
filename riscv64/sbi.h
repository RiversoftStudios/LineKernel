/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#ifndef __SBI_H__
#define __SBI_H__

#define SBI_EXT_SRST 0x53525354
#define SBI_EXT_SRST_RESET 0
#define SBI_SRST_RESET_TYPE_SHUTDOWN 0
#define SBI_SRST_RESET_TYPE_COLD_REBOOT 1
#define SBI_SRST_RESET_REASON_NONE 0

struct sbiret {
	long error;
	long value;
};

struct sbiret sbi_call(long arg0, long arg1, long arg2, long arg3, long arg4, long arg5, long fid,
	long eid);
void sbi_putchar(char ch);
int sbi_getchar(void);

void sbi_shutdown(void);
void sbi_reboot(void);

#endif
