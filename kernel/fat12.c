/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#include "fat12.h"
#include "str.h"
#include "fileno.h"

#ifdef CONFIG_FAT12

extern drivesformat_t drive;

struct fat12_geometry {
	uint16_t bytes_per_sector;
	uint8_t sectors_per_cluster;
	uint16_t reserved_sectors;
	uint8_t num_fats;
	uint16_t root_dir_entries;
	uint16_t sectors_per_fat;
	uint32_t fat_start_sector;
	uint32_t root_dir_start_sector;
	uint32_t root_dir_sectors;
	uint32_t data_start_sector;
} __attribute__((packed));

static struct fat12_geometry fat12_geom;

int detect_fat12(drivesformat_t drive_id)
{
	uint8_t sector[512];

	/* Attempt to read sector 0 */
	if (read_drive_sector(drive_id, 0, (uint32_t*)sector) < 0) {
		printf("No drive detected or failed to read sector 0.\n");
		return -1;				/* No drive or read error */
	}

	/* Verify boot sector signature (0xAA55) */
	uint16_t boot_sig = sector[510] | (sector[511] << 8);

	if (boot_sig != 0xAA55) {
		printf("Drive present, but boot signature 0x%04X is invalid.\n", boot_sig);
		return 0;				/* Not a valid boot sector */
	}

	struct fat12_bpb* bpb = (struct fat12_bpb *)sector;

	/* Perform basic validations of FAT12 geometry */
	int is_fat12 = 1;

	if (bpb->bytes_per_sector != 512) {
		is_fat12 = 0;
	}
	if (bpb->media_descriptor != 0xF0 && bpb->media_descriptor != 0xF9) {
		/* 0xF0 = 1.44MB, 0xF9 = 720KB/1.2MB */
		is_fat12 = 0;
	}
	if (bpb->fs_type[0] != 'F' || bpb->fs_type[1] != 'A' || bpb->fs_type[2] != 'T' ||
		bpb->fs_type[3] != '1' || bpb->fs_type[4] != '2') {
		is_fat12 = 0;
	}

	if (is_fat12) {
		char oem[9];
		char label[12];

		for (int i = 0; i < 8; i++) {
			oem[i] = bpb->oem_id[i];
		}
		oem[8] = '\0';

		for (int i = 0; i < 11; i++) {
			label[i] = bpb->volume_label[i];
		}
		label[11] = '\0';

		fat12_geom.bytes_per_sector = bpb->bytes_per_sector;
		fat12_geom.sectors_per_cluster = bpb->sectors_per_cluster;
		fat12_geom.reserved_sectors = bpb->reserved_sectors;
		fat12_geom.num_fats = bpb->num_fats;
		fat12_geom.root_dir_entries = bpb->root_dir_entries;
		fat12_geom.sectors_per_fat = bpb->sectors_per_fat;

		fat12_geom.fat_start_sector = bpb->reserved_sectors;
		fat12_geom.root_dir_start_sector = bpb->reserved_sectors + (bpb->num_fats * bpb->sectors_per_fat);
		fat12_geom.root_dir_sectors = ((bpb->root_dir_entries * 32) + bpb->bytes_per_sector - 1) / bpb->bytes_per_sector;
		fat12_geom.data_start_sector = fat12_geom.root_dir_start_sector + fat12_geom.root_dir_sectors;

		printf("Valid FAT12 filesystem detected!\n");
		printf("  OEM ID:           %s\n", oem);
		printf("  Volume Label:     %s\n", label);
		printf("  Bytes/Sector:     %d\n", bpb->bytes_per_sector);
		printf("  Sectors/Cluster:  %d\n", bpb->sectors_per_cluster);
		printf("  Reserved Sectors: %d\n", bpb->reserved_sectors);
		printf("  Sectors/FAT:      %d\n", bpb->sectors_per_fat);
		printf("  Heads:            %d\n", bpb->num_heads);
		printf("  Sectors/Track:    %d\n", bpb->sectors_per_track);
		return 1;
	}

	printf("Disk present but does not contain a valid FAT12 filesystem.\n");
	return 0;
}

static void read_fat12_fat_bytes(uint32_t byte_offset, uint8_t *bytes, int count)
{
	for (int i = 0; i < count; i++) {
		uint32_t bo = byte_offset + i;
		uint32_t sector = fat12_geom.fat_start_sector + (bo / 512);
		uint32_t offset_in_sector = bo % 512;
		uint8_t sec_buf[512];
		read_drive_sector(drive, sector, (uint32_t*)sec_buf);
		bytes[i] = sec_buf[offset_in_sector];
	}
}

static void write_fat12_fat_bytes(uint32_t byte_offset, const uint8_t *bytes, int count)
{
	for (int i = 0; i < count; i++) {
		uint32_t bo = byte_offset + i;
		uint32_t sector = fat12_geom.fat_start_sector + (bo / 512);
		uint32_t offset_in_sector = bo % 512;
		uint8_t sec_buf[512];
		read_drive_sector(drive, sector, (uint32_t*)sec_buf);
		sec_buf[offset_in_sector] = bytes[i];
		write_drive_sector(drive, sector, (const uint32_t*)sec_buf);
		
		for (int f = 1; f < fat12_geom.num_fats; f++) {
			uint32_t dup_sector = sector + f * fat12_geom.sectors_per_fat;
			write_drive_sector(drive, dup_sector, (const uint32_t*)sec_buf);
		}
	}
}

static uint16_t fat12_read_fat_entry(uint32_t cluster)
{
	uint32_t byte_offset = (cluster * 3) / 2;
	uint8_t bytes[2];
	read_fat12_fat_bytes(byte_offset, bytes, 2);
	uint16_t entry = bytes[0] | (bytes[1] << 8);
	if (cluster & 1) {
		return entry >> 4;
	} else {
		return entry & 0x0FFF;
	}
}

static void fat12_write_fat_entry(uint32_t cluster, uint16_t value)
{
	uint32_t byte_offset = (cluster * 3) / 2;
	uint8_t bytes[2];
	read_fat12_fat_bytes(byte_offset, bytes, 2);
	uint16_t entry = bytes[0] | (bytes[1] << 8);
	if (cluster & 1) {
		entry = (entry & 0x000F) | (value << 4);
	} else {
		entry = (entry & 0xF000) | (value & 0x0FFF);
	}
	bytes[0] = entry & 0xFF;
	bytes[1] = (entry >> 8) & 0xFF;
	write_fat12_fat_bytes(byte_offset, bytes, 2);
}

static int fat12_allocate_cluster(uint32_t current_cluster)
{
	for (uint32_t c = 2; c < 4080; c++) {
		if (fat12_read_fat_entry(c) == 0) {
			fat12_write_fat_entry(c, 0x0FFF);
			if (current_cluster >= 2) {
				fat12_write_fat_entry(current_cluster, c);
			}
			return c;
		}
	}
	return -1;
}

static void fat12_free_cluster_chain(uint32_t start_cluster)
{
	uint32_t c = start_cluster;
	while (c >= 2 && c < 0x0FF8) {
		uint32_t next = fat12_read_fat_entry(c);
		fat12_write_fat_entry(c, 0x000);
		c = next;
	}
}

static int fat12_read_dir_entry(uint32_t dir_cluster, uint32_t entry_index, struct fat12_dir_entry *out_entry)
{
	if (dir_cluster == 0) {
		if (entry_index >= fat12_geom.root_dir_entries) {
			return -1;
		}
		uint32_t byte_offset = entry_index * 32;
		uint32_t sector = fat12_geom.root_dir_start_sector + (byte_offset / 512);
		uint32_t offset_in_sector = byte_offset % 512;
		uint8_t sec_buf[512];
		if (read_drive_sector(drive, sector, (uint32_t*)sec_buf) < 0) {
			return -1;
		}
		memcpy(out_entry, sec_buf + offset_in_sector, 32);
		return 0;
	} else {
		uint32_t entries_per_cluster = (fat12_geom.sectors_per_cluster * 512) / 32;
		uint32_t target_cluster_index = entry_index / entries_per_cluster;
		uint32_t entry_offset_in_cluster = entry_index % entries_per_cluster;

		uint32_t c = dir_cluster;
		for (uint32_t i = 0; i < target_cluster_index; i++) {
			c = fat12_read_fat_entry(c);
			if (c < 2 || c >= 0x0FF8) {
				return -1;
			}
		}

		uint32_t byte_offset = entry_offset_in_cluster * 32;
		uint32_t sector_offset = byte_offset / 512;
		uint32_t offset_in_sector = byte_offset % 512;

		uint32_t sector = fat12_geom.data_start_sector + (c - 2) * fat12_geom.sectors_per_cluster + sector_offset;
		uint8_t sec_buf[512];
		if (read_drive_sector(drive, sector, (uint32_t*)sec_buf) < 0) {
			return -1;
		}
		memcpy(out_entry, sec_buf + offset_in_sector, 32);
		return 0;
	}
}

static int fat12_write_dir_entry(uint32_t dir_cluster, uint32_t entry_index, const struct fat12_dir_entry *in_entry)
{
	if (dir_cluster == 0) {
		if (entry_index >= fat12_geom.root_dir_entries) {
			return -1;
		}
		uint32_t byte_offset = entry_index * 32;
		uint32_t sector = fat12_geom.root_dir_start_sector + (byte_offset / 512);
		uint32_t offset_in_sector = byte_offset % 512;
		uint8_t sec_buf[512];
		if (read_drive_sector(drive, sector, (uint32_t*)sec_buf) < 0) {
			return -1;
		}
		memcpy(sec_buf + offset_in_sector, in_entry, 32);
		if (write_drive_sector(drive, sector, (const uint32_t*)sec_buf) < 0) {
			return -1;
		}
		return 0;
	} else {
		uint32_t entries_per_cluster = (fat12_geom.sectors_per_cluster * 512) / 32;
		uint32_t target_cluster_index = entry_index / entries_per_cluster;
		uint32_t entry_offset_in_cluster = entry_index % entries_per_cluster;

		uint32_t c = dir_cluster;
		uint32_t prev_c = 0;
		for (uint32_t i = 0; i < target_cluster_index; i++) {
			prev_c = c;
			c = fat12_read_fat_entry(c);
			if (c < 2 || c >= 0x0FF8) {
				int new_c = fat12_allocate_cluster(prev_c);
				if (new_c < 0) {
					return -1;
				}
				uint8_t zero_buf[512];
				memset(zero_buf, 0, 512);
				for (uint32_t s = 0; s < fat12_geom.sectors_per_cluster; s++) {
					uint32_t sector = fat12_geom.data_start_sector + (new_c - 2) * fat12_geom.sectors_per_cluster + s;
					write_drive_sector(drive, sector, (const uint32_t*)zero_buf);
				}
				c = new_c;
			}
		}

		uint32_t byte_offset = entry_offset_in_cluster * 32;
		uint32_t sector_offset = byte_offset / 512;
		uint32_t offset_in_sector = byte_offset % 512;

		uint32_t sector = fat12_geom.data_start_sector + (c - 2) * fat12_geom.sectors_per_cluster + sector_offset;
		uint8_t sec_buf[512];
		if (read_drive_sector(drive, sector, (uint32_t*)sec_buf) < 0) {
			return -1;
		}
		memcpy(sec_buf + offset_in_sector, in_entry, 32);
		if (write_drive_sector(drive, sector, (const uint32_t*)sec_buf) < 0) {
			return -1;
		}
		return 0;
	}
}

static void to_fat12_name(const char *src, char *dest_name, char *dest_ext)
{
	for (int i = 0; i < 8; i++) dest_name[i] = ' ';
	for (int i = 0; i < 3; i++) dest_ext[i] = ' ';

	int i = 0;
	while (src[i] != '\0' && src[i] != '.' && i < 8) {
		char c = src[i];
		if (c >= 'a' && c <= 'z') c = c - 'a' + 'A';
		dest_name[i] = c;
		i++;
	}

	while (src[i] != '\0' && src[i] != '.') {
		i++;
	}

	if (src[i] == '.') {
		i++;
		int j = 0;
		while (src[i] != '\0' && j < 3) {
			char c = src[i];
			if (c >= 'a' && c <= 'z') c = c - 'a' + 'A';
			dest_ext[j] = c;
			i++;
			j++;
		}
	}
}

static int match_fat12_name(const struct fat12_dir_entry *entry, const char *name, const char *ext)
{
	for (int i = 0; i < 8; i++) {
		if (entry->name[i] != name[i]) return 0;
	}
	for (int i = 0; i < 3; i++) {
		if (entry->ext[i] != ext[i]) return 0;
	}
	return 1;
}

static int get_next_path_component(const char *path, int *offset, char *component)
{
	while (path[*offset] == '/') {
		(*offset)++;
	}
	if (path[*offset] == '\0') {
		return 0;
	}
	int len = 0;
	while (path[*offset] != '\0' && path[*offset] != '/') {
		if (len < 255) {
			component[len++] = path[*offset];
		}
		(*offset)++;
	}
	component[len] = '\0';
	return 1;
}

static int fat12_find_entry(const char *path, struct fat12_dir_entry *out_entry, uint32_t *out_entry_index, uint32_t *out_dir_cluster)
{
	uint32_t dir_cluster = 0;
	int offset = 0;
	char component[256];

	if (!get_next_path_component(path, &offset, component)) {
		return -9;
	}

	while (1) {
		char formatted_name[8];
		char formatted_ext[3];
		to_fat12_name(component, formatted_name, formatted_ext);

		int found = 0;
		struct fat12_dir_entry entry;
		uint32_t entry_idx = 0;
		while (1) {
			if (fat12_read_dir_entry(dir_cluster, entry_idx, &entry) < 0) {
				break;
			}
			if (entry.name[0] == 0x00) {
				break;
			}
			if (entry.name[0] == (char)0xE5) {
				entry_idx++;
				continue;
			}
			if (entry.attr == 0x0F) {
				entry_idx++;
				continue;
			}
			if (match_fat12_name(&entry, formatted_name, formatted_ext)) {
				found = 1;
				break;
			}
			entry_idx++;
		}

		if (!found) {
			return -9;
		}

		int next_offset = offset;
		char next_component[256];
		int has_more = get_next_path_component(path, &next_offset, next_component);

		if (has_more) {
			if (!(entry.attr & 0x10)) {
				return -9;
			}
			dir_cluster = entry.first_cluster_low;
			offset = next_offset;
			strlcpy(component, next_component, sizeof(component));
		} else {
			if (out_entry) *out_entry = entry;
			if (out_entry_index) *out_entry_index = entry_idx;
			if (out_dir_cluster) *out_dir_cluster = dir_cluster;
			return 0;
		}
	}
}

static int fat12_resolve_parent(const char *path, uint32_t *out_parent_cluster, char *final_name, char *final_ext)
{
	uint32_t dir_cluster = 0;
	int offset = 0;
	char component[256];

	if (!get_next_path_component(path, &offset, component)) {
		return -9;
	}

	while (1) {
		int next_offset = offset;
		char next_component[256];
		int has_more = get_next_path_component(path, &next_offset, next_component);

		if (has_more) {
			char formatted_name[8];
			char formatted_ext[3];
			to_fat12_name(component, formatted_name, formatted_ext);

			int found = 0;
			struct fat12_dir_entry entry;
			uint32_t entry_idx = 0;
			while (1) {
				if (fat12_read_dir_entry(dir_cluster, entry_idx, &entry) < 0) {
					break;
				}
				if (entry.name[0] == 0x00) {
					break;
				}
				if (entry.name[0] == (char)0xE5) {
					entry_idx++;
					continue;
				}
				if (entry.attr == 0x0F) {
					entry_idx++;
					continue;
				}
				if (match_fat12_name(&entry, formatted_name, formatted_ext)) {
					found = 1;
					break;
				}
				entry_idx++;
			}

			if (!found) {
				return -9;
			}
			if (!(entry.attr & 0x10)) {
				return -9;
			}
			dir_cluster = entry.first_cluster_low;
			offset = next_offset;
			strlcpy(component, next_component, sizeof(component));
		} else {
			to_fat12_name(component, final_name, final_ext);
			*out_parent_cluster = dir_cluster;
			return 0;
		}
	}
}

static int fat12_create_entry(uint32_t parent_cluster, const char *name, const char *ext, uint8_t attr, uint32_t first_cluster, uint32_t size, struct fat12_dir_entry *out_entry)
{
	struct fat12_dir_entry entry;
	uint32_t entry_idx = 0;
	int slot_found = 0;

	while (1) {
		if (fat12_read_dir_entry(parent_cluster, entry_idx, &entry) < 0) {
			if (parent_cluster == 0) {
				return -1;
			}
			slot_found = 1;
			break;
		}
		if (entry.name[0] == 0x00 || entry.name[0] == (char)0xE5) {
			slot_found = 1;
			break;
		}
		entry_idx++;
	}

	if (slot_found) {
		struct fat12_dir_entry check_entry;
		int was_end = 0;
		if (fat12_read_dir_entry(parent_cluster, entry_idx, &check_entry) == 0 && check_entry.name[0] == 0x00) {
			was_end = 1;
		}

		struct fat12_dir_entry new_entry;
		memset(&new_entry, 0, sizeof(new_entry));
		for (int i = 0; i < 8; i++) new_entry.name[i] = name[i];
		for (int i = 0; i < 3; i++) new_entry.ext[i] = ext[i];
		new_entry.attr = attr;
		new_entry.first_cluster_low = first_cluster;
		new_entry.file_size = size;

		if (fat12_write_dir_entry(parent_cluster, entry_idx, &new_entry) < 0) {
			return -1;
		}

		if (was_end) {
			struct fat12_dir_entry end_entry;
			memset(&end_entry, 0, sizeof(end_entry));
			fat12_write_dir_entry(parent_cluster, entry_idx + 1, &end_entry);
		}
		if (out_entry) *out_entry = new_entry;
		return entry_idx;
	}
	return -1;
}

int fat12_open(const char* filename)
{
	int fd = -1;
	for (int i = 0; i < MAX_OPEN_FILES; i++) {
		int candidate_fd = i + 3;
		if (!open_files[i].used) {
			fd = candidate_fd;
			break;
		}
	}
	if (fd == -1) {
		return -1;
	}

	struct fat12_dir_entry entry;
	uint32_t entry_idx, parent_cluster;
	int exists = (fat12_find_entry(filename, &entry, &entry_idx, &parent_cluster) == 0);

	if (exists) {
		if (entry.attr & 0x10) {
			return -1;
		}
	} else {
		char name[8], ext[3];
		if (fat12_resolve_parent(filename, &parent_cluster, name, ext) != 0) {
			return -9;
		}
		int idx = fat12_create_entry(parent_cluster, name, ext, 0, 0, 0, &entry);
		if (idx < 0) {
			return -1;
		}
		entry_idx = idx;
	}

	int index = fd - 3;
	open_files[index].used = 1;
	strlcpy(open_files[index].path, filename, sizeof(open_files[index].path));
	open_files[index].first_cluster = entry.first_cluster_low;
	open_files[index].current_cluster = entry.first_cluster_low;
	open_files[index].cursor = 0;
	open_files[index].file_size = entry.file_size;
	open_files[index].parent_cluster = parent_cluster;
	open_files[index].entry_idx = entry_idx;

	return fd;
}

int fat12_close(int fileno)
{
	int index = fileno - 3;
	if (index < 0 || index >= MAX_OPEN_FILES || !open_files[index].used) {
		return -5;
	}
	open_files[index].used = 0;
	return 0;
}

int fat12_write(int fileno, const void* buf, size_t nbyte)
{
	int index = fileno - 3;
	if (index < 0 || index >= MAX_OPEN_FILES || !open_files[index].used) {
		return -5;
	}

	const uint8_t *src = (const uint8_t *)buf;
	size_t bytes_written = 0;

	uint32_t cluster_size = fat12_geom.sectors_per_cluster * 512;

	while (bytes_written < nbyte) {
		uint32_t cursor = open_files[index].cursor;
		uint32_t cluster_index = cursor / cluster_size;
		uint32_t offset_in_cluster = cursor % cluster_size;

		if (open_files[index].first_cluster == 0) {
			int new_c = fat12_allocate_cluster(0);
			if (new_c < 0) {
				return -1;
			}
			open_files[index].first_cluster = new_c;

			struct fat12_dir_entry entry;
			if (fat12_read_dir_entry(open_files[index].parent_cluster, open_files[index].entry_idx, &entry) == 0) {
				entry.first_cluster_low = new_c;
				fat12_write_dir_entry(open_files[index].parent_cluster, open_files[index].entry_idx, &entry);
			}
		}

		uint32_t c = open_files[index].first_cluster;
		int ok = 1;
		for (uint32_t i = 0; i < cluster_index; i++) {
			uint32_t next = fat12_read_fat_entry(c);
			if (next < 2 || next >= 0x0FF8) {
				int new_c = fat12_allocate_cluster(c);
				if (new_c < 0) {
					ok = 0;
					break;
				}
				next = new_c;
			}
			c = next;
		}
		if (!ok) {
			break;
		}

		uint32_t sector_offset = offset_in_cluster / 512;
		uint32_t offset_in_sector = offset_in_cluster % 512;
		uint32_t sector = fat12_geom.data_start_sector + (c - 2) * fat12_geom.sectors_per_cluster + sector_offset;

		uint8_t sec_buf[512];
		if (read_drive_sector(drive, sector, (uint32_t*)sec_buf) < 0) {
			return -1;
		}

		uint32_t space_in_sector = 512 - offset_in_sector;
		uint32_t to_write = nbyte - bytes_written;
		if (to_write > space_in_sector) {
			to_write = space_in_sector;
		}

		for (uint32_t i = 0; i < to_write; i++) {
			sec_buf[offset_in_sector + i] = src[bytes_written + i];
		}

		if (write_drive_sector(drive, sector, (const uint32_t*)sec_buf) < 0) {
			return -1;
		}

		bytes_written += to_write;
		open_files[index].cursor += to_write;
		if (open_files[index].cursor > open_files[index].file_size) {
			open_files[index].file_size = open_files[index].cursor;

			struct fat12_dir_entry entry;
			if (fat12_read_dir_entry(open_files[index].parent_cluster, open_files[index].entry_idx, &entry) == 0) {
				entry.file_size = open_files[index].file_size;
				fat12_write_dir_entry(open_files[index].parent_cluster, open_files[index].entry_idx, &entry);
			}
		}
	}

	return bytes_written;
}

int fat12_rename(const char* oldpath, const char* newpath)
{
	struct fat12_dir_entry old_entry;
	uint32_t old_entry_idx, old_parent_cluster;

	if (fat12_find_entry(oldpath, &old_entry, &old_entry_idx, &old_parent_cluster) != 0) {
		return -9;
	}

	struct fat12_dir_entry existing_entry;
	if (fat12_find_entry(newpath, &existing_entry, NULL, NULL) == 0) {
		return -1;
	}

	uint32_t new_parent_cluster;
	char new_name[8], new_ext[3];
	if (fat12_resolve_parent(newpath, &new_parent_cluster, new_name, new_ext) != 0) {
		return -9;
	}

	int new_entry_idx = fat12_create_entry(new_parent_cluster, new_name, new_ext, old_entry.attr, old_entry.first_cluster_low, old_entry.file_size, NULL);
	if (new_entry_idx < 0) {
		return -1;
	}

	if ((old_entry.attr & 0x10) && old_entry.first_cluster_low >= 2) {
		struct fat12_dir_entry dotdot;
		if (fat12_read_dir_entry(old_entry.first_cluster_low, 1, &dotdot) == 0) {
			dotdot.first_cluster_low = new_parent_cluster;
			fat12_write_dir_entry(old_entry.first_cluster_low, 1, &dotdot);
		}
	}

	old_entry.name[0] = (char)0xE5;
	fat12_write_dir_entry(old_parent_cluster, old_entry_idx, &old_entry);

	for (int i = 0; i < MAX_OPEN_FILES; i++) {
		if (open_files[i].used && strcmp(open_files[i].path, oldpath) == 0) {
			strlcpy(open_files[i].path, newpath, sizeof(open_files[i].path));
		}
	}

	return 0;
}

static void fat12_init_dir_cluster(uint32_t cluster, uint32_t parent_cluster)
{
	struct fat12_dir_entry dot;
	struct fat12_dir_entry dotdot;

	memset(&dot, 0, sizeof(dot));
	memset(&dotdot, 0, sizeof(dotdot));

	memset(dot.name, ' ', 8);
	memset(dot.ext, ' ', 3);
	memset(dotdot.name, ' ', 8);
	memset(dotdot.ext, ' ', 3);

	dot.name[0] = '.';
	dot.attr = 0x10;
	dot.first_cluster_low = cluster;

	dotdot.name[0] = '.';
	dotdot.name[1] = '.';
	dotdot.attr = 0x10;
	dotdot.first_cluster_low = parent_cluster;

	fat12_write_dir_entry(cluster, 0, &dot);
	fat12_write_dir_entry(cluster, 1, &dotdot);

	struct fat12_dir_entry end_entry;
	memset(&end_entry, 0, sizeof(end_entry));
	fat12_write_dir_entry(cluster, 2, &end_entry);
}

int fat12_makedir(const char* path)
{
	uint32_t parent_cluster;
	char name[8], ext[3];
	if (fat12_resolve_parent(path, &parent_cluster, name, ext) != 0) {
		return -9;
	}

	struct fat12_dir_entry existing;
	if (fat12_find_entry(path, &existing, NULL, NULL) == 0) {
		return -1;
	}

	int new_cluster = fat12_allocate_cluster(0);
	if (new_cluster < 0) {
		return -1;
	}

	fat12_init_dir_cluster(new_cluster, parent_cluster);

	int idx = fat12_create_entry(parent_cluster, name, ext, 0x10, new_cluster, 0, NULL);
	if (idx < 0) {
		fat12_free_cluster_chain(new_cluster);
		return -1;
	}

	return 0;
}

static int fat12_is_dir_empty(uint32_t dir_cluster)
{
	struct fat12_dir_entry entry;
	uint32_t entry_idx = 0;
	while (1) {
		if (fat12_read_dir_entry(dir_cluster, entry_idx, &entry) < 0) {
			break;
		}
		if (entry.name[0] == 0x00) {
			break;
		}
		if (entry.name[0] == (char)0xE5 || entry.attr == 0x0F) {
			entry_idx++;
			continue;
		}
		if (entry.name[0] == '.' && (entry.name[1] == ' ' || (entry.name[1] == '.' && entry.name[2] == ' '))) {
			entry_idx++;
			continue;
		}
		return 0;
	}
	return 1;
}

int fat12_removedir(const char* path)
{
	struct fat12_dir_entry entry;
	uint32_t entry_idx, parent_cluster;

	if (fat12_find_entry(path, &entry, &entry_idx, &parent_cluster) != 0) {
		return -9;
	}
	if (!(entry.attr & 0x10)) {
		return -9;
	}

	if (!fat12_is_dir_empty(entry.first_cluster_low)) {
		return -11;
	}

	if (entry.first_cluster_low >= 2) {
		fat12_free_cluster_chain(entry.first_cluster_low);
	}

	entry.name[0] = (char)0xE5;
	if (fat12_write_dir_entry(parent_cluster, entry_idx, &entry) < 0) {
		return -1;
	}

	return 0;
}

int fat12_removefile(const char* filename)
{
	struct fat12_dir_entry entry;
	uint32_t entry_idx, parent_cluster;

	if (fat12_find_entry(filename, &entry, &entry_idx, &parent_cluster) != 0) {
		return -9;
	}
	if (entry.attr & 0x10) {
		return -9;
	}

	for (int i = 0; i < MAX_OPEN_FILES; i++) {
		if (open_files[i].used && (open_files[i].first_cluster == entry.first_cluster_low)) {
			if (entry.first_cluster_low != 0 || strcmp(open_files[i].path, filename) == 0) {
				return -11;
			}
		}
	}

	if (entry.first_cluster_low >= 2) {
		fat12_free_cluster_chain(entry.first_cluster_low);
	}

	entry.name[0] = (char)0xE5;
	if (fat12_write_dir_entry(parent_cluster, entry_idx, &entry) < 0) {
		return -1;
	}

	return 0;
}

ptrdiff_t fat12_read(int fileno, void* buf, size_t count)
{
	int index = fileno - 3;
	if (index < 0 || index >= MAX_OPEN_FILES || !open_files[index].used) {
		return -5;
	}

	uint8_t *dest = (uint8_t *)buf;
	size_t bytes_read = 0;
	uint32_t cluster_size = fat12_geom.sectors_per_cluster * 512;

	while (bytes_read < count) {
		uint32_t cursor = open_files[index].cursor;
		/* EOF */
		if (cursor >= open_files[index].file_size) {
			break;
		}

		uint32_t cluster_index = cursor / cluster_size;
		uint32_t offset_in_cluster = cursor % cluster_size;

		uint32_t c = open_files[index].first_cluster;
		if (c < 2) {
			break;
		}

		int ok = 1;
		for (uint32_t i = 0; i < cluster_index; i++) {
			uint32_t next = fat12_read_fat_entry(c);
			if (next < 2 || next >= 0x0FF8) {
				ok = 0;
				break;
			}
			c = next;
		}
		if (!ok) {
			break;
		}

		uint32_t sector_offset = offset_in_cluster / 512;
		uint32_t offset_in_sector = offset_in_cluster % 512;
		uint32_t sector = fat12_geom.data_start_sector + (c - 2) * fat12_geom.sectors_per_cluster + sector_offset;

		uint8_t sec_buf[512];
		if (read_drive_sector(drive, sector, (uint32_t*)sec_buf) < 0) {
			return -1;
		}

		uint32_t space_in_sector = 512 - offset_in_sector;
		uint32_t to_read = count - bytes_read;
		if (to_read > space_in_sector) {
			to_read = space_in_sector;
		}
		uint32_t remaining_in_file = open_files[index].file_size - cursor;
		if (to_read > remaining_in_file) {
			to_read = remaining_in_file;
		}

		for (uint32_t i = 0; i < to_read; i++) {
			dest[bytes_read + i] = sec_buf[offset_in_sector + i];
		}

		bytes_read += to_read;
		open_files[index].cursor += to_read;
	}

	return bytes_read;
}

static void from_fat12_name(const char *fat_name, const char *fat_ext, char *dest)
{
	int len = 0;
	int name_len = 8;
	while (name_len > 0 && fat_name[name_len - 1] == ' ') {
		name_len--;
	}
	for (int i = 0; i < name_len; i++) {
		dest[len++] = fat_name[i];
	}

	int ext_len = 3;
	while (ext_len > 0 && fat_ext[ext_len - 1] == ' ') {
		ext_len--;
	}
	if (ext_len > 0) {
		dest[len++] = '.';
		for (int i = 0; i < ext_len; i++) {
			dest[len++] = fat_ext[i];
		}
	}
	dest[len] = '\0';
}

static char contents_filenames[64][32];
static int contents_filename_idx = 0;

int fat12_listcontent(const char* path, contents_t* fcontents, size_t count)
{
	size_t populated_count = 0;
	uint32_t dir_cluster = 0;
	int is_dir = 0;

	if (strcmp(path, "/") == 0 || strcmp(path, "") == 0 || strcmp(path, "/.") == 0) {
		is_dir = 1;
		dir_cluster = 0;
	} else {
		struct fat12_dir_entry target_entry;
		uint32_t entry_idx, parent_cluster;

		if (fat12_find_entry(path, &target_entry, &entry_idx, &parent_cluster) != 0) {
			return -9;
		}

		if (target_entry.attr & 0x10) {
			is_dir = 1;
			dir_cluster = target_entry.first_cluster_low;
		} else {
			is_dir = 0;
			if (count > 0) {
				fcontents[0].content_type = FILE;
				char* buf = contents_filenames[contents_filename_idx];
				contents_filename_idx = (contents_filename_idx + 1) % 64;
				from_fat12_name(target_entry.name, target_entry.ext, buf);
				fcontents[0].filename = buf;
				populated_count = 1;
			}
		}
	}

	if (is_dir) {
		uint32_t read_idx = 0;
		struct fat12_dir_entry dir_entry;

		while (populated_count < count) {
			if (fat12_read_dir_entry(dir_cluster, read_idx, &dir_entry) < 0) {
				break;
			}
			if (dir_entry.name[0] == 0x00) {
				break;
			}
			if (dir_entry.name[0] == (char)0xE5 || dir_entry.attr == 0x0F) {
				read_idx++;
				continue;
			}
			if (dir_entry.name[0] == '.' && 
				(dir_entry.name[1] == ' ' || (dir_entry.name[1] == '.' && dir_entry.name[2] == ' '))) {
				read_idx++;
				continue;
			}

			fcontents[populated_count].content_type = (dir_entry.attr & 0x10) ? DIRECTORY : FILE;
			char* buf = contents_filenames[contents_filename_idx];
			contents_filename_idx = (contents_filename_idx + 1) % 64;
			from_fat12_name(dir_entry.name, dir_entry.ext, buf);
			fcontents[populated_count].filename = buf;

			populated_count++;
			read_idx++;
		}
	}

	if (populated_count < count) {
		fcontents[populated_count].filename = NULL;
	}

	return 0;
}

size_t fat12_get_size(int fileno)
{
	int index = fileno - 3;
	if (index < 0 || index >= MAX_OPEN_FILES || !open_files[index].used) {
		return 0;
	}
	return open_files[index].file_size;
}

#endif
