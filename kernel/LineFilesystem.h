/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#ifndef __LINEFILESYSTEM_H__
#define __LINEFILESYSTEM_H__

#include "LineDrive.h"
#include "fat12.h"
#include "kconfig.h"
#include "LineKernel/predefined_fileno.h"
#include "LineKernel/contents_t.h"
#include <stddef.h>

enum filesystemformat {
	nofilesystem,
	fat12,
};

typedef enum filesystemformat filesystemformat_t;

const char* filesystem_name(filesystemformat_t fs);
int check_filesystem(drivesformat_t drive, filesystemformat_t fs);

int close_fileno(int fileno);
int open_fileno(const char* filename);
int rename_filepath(const char* oldpath, const char* newpath);
int makedir(const char* path);
int removedir(const char* path);
int removefile(const char* path);

/* ssize_t isn't defined, so this will have to do for now... */
ptrdiff_t read_fileno(int fileno, void* buf, size_t count);
ptrdiff_t write_fileno(int fileno, const void* buf, size_t nbyte);

int list_contents(const char* path, contents_t* fcontents, size_t count);
size_t get_size(int fileno);

#endif
