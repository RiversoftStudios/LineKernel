/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#ifndef __FAT12_H__
#define __FAT12_H__

#include "kconfig.h"

#ifdef CONFIG_FAT12
#include <stdint.h>
#include "LineDrive.h"
#include "printf.h"
#include "LineKernel/contents_t.h"
#include <stddef.h>

struct fat12_bpb {
	uint8_t jmp[3];
	char oem_id[8];
	uint16_t bytes_per_sector;
	uint8_t sectors_per_cluster;
	uint16_t reserved_sectors;
	uint8_t num_fats;
	uint16_t root_dir_entries;
	uint16_t total_sectors_small;
	uint8_t media_descriptor;
	uint16_t sectors_per_fat;
	uint16_t sectors_per_track;
	uint16_t num_heads;
	uint32_t hidden_sectors;
	uint32_t total_sectors_large;
	uint8_t drive_number;
	uint8_t reserved;
	uint8_t boot_signature;
	uint32_t volume_id;
	char volume_label[11];
	char fs_type[8];
} __attribute__((packed));

struct fat12_dir_entry {
	char name[8];
	char ext[3];
	uint8_t attr;
	uint8_t lcase;
	uint8_t creation_time_ms;
	uint16_t creation_time;
	uint16_t creation_date;
	uint16_t last_access_date;
	uint16_t first_cluster_high;
	uint16_t last_write_time;
	uint16_t last_write_date;
	uint16_t first_cluster_low;
	uint32_t file_size;
} __attribute__((packed));

int detect_fat12(drivesformat_t drive);
int fat12_open(const char* filename);
int fat12_close(int fileno);
int fat12_write(int fileno, const void* buf, size_t nbyte);
int fat12_rename(const char* oldpath, const char* newpath);
int fat12_makedir(const char* path);
int fat12_removedir(const char* path);
int fat12_removefile(const char* filename);
ptrdiff_t fat12_read(int fileno, void* buf, size_t count);
int fat12_listcontent(const char* path, contents_t * fcontents, size_t count);
size_t fat12_get_size(int fileno);
#endif

#endif
