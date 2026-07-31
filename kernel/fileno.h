/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#ifndef __FILENO_H__
#define __FILENO_H__

#include <stdint.h>

#define MAX_OPEN_FILES 256

struct open_file {
	int used;
	char path[256];
	uint32_t first_cluster;
	uint32_t current_cluster;
	uint32_t cursor;
	uint32_t file_size;
	uint32_t parent_cluster;
	uint32_t entry_idx;
};

extern struct open_file open_files[MAX_OPEN_FILES];

#endif
