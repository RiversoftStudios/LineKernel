/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#include "prekernel.h"

void multiboot_pre_main(uint32_t magic, uint32_t mbi_address)
{
	char* cmdline_str_;

	multiboot_info_t* mbi = (multiboot_info_t *) (uintptr_t) mbi_address;

	if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
		return;

	if (MULTIBOOT_CHECK_FLAG(mbi->flags, 2)) {
		cmdline_str_ = (char *)(uintptr_t) mbi->cmdline;
		strlcpy(cmdline_str, cmdline_str_, strlen(cmdline_str_) + 1);
		parse_cmdline(cmdline_str_);
	}

	kernel_main();
}
