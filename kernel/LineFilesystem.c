/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

/* NOTE! Functions without `drivesformat_t drives` OR `filesystemformat_t filesystem` will
 * simply just use the `fs` variable, which is the "current filesystem".
 * Note that if `has_working_drive != 1` then it will fail with `return -32`. */

#include "LineFilesystem.h"
#include "LineKernel.h"
#include "LineRenderer.h"

const char* filesystem_name(filesystemformat_t filesystem)
{
	switch (filesystem) {
		case fat12:
			return "FAT12";
		default:
			return "(none)";
	}
}

/** @retval 0 Success
 ** @retval other Failure */
int check_filesystem(drivesformat_t drives, filesystemformat_t filesystem)
{
	switch (filesystem) {
#ifdef CONFIG_FAT12
		case fat12:
			return detect_fat12(drives);
#endif
		default:
			return -1;
	}
}

/** @retval 0 Success
 ** @retval -32 No drive
 ** @retval -5 Bad fileno
 ** @retval other Unknown error */
int close_fileno(int fileno)
{
	if (has_working_drive != 1) return -32;
	if (fileno == LINEOUT_FILENO) return -5;
	switch (fs) {
#ifdef CONFIG_FAT12
		case fat12:
			return fat12_close(fileno);
#endif
		default:
			return -1;
	}
}

/** @retval -1 Unknown error
 ** @retval -32 No drive
 ** @retval -9 No such file
 ** @retval 0 Not possible
 ** @retval other fileno */
int open_fileno(const char* filename)
{
	if (has_working_drive != 1) return -32;
	switch (fs) {
#ifdef CONFIG_FAT12
		case fat12:
			return fat12_open(filename);
#endif
		default:
			return -1;
	}
}

/** @retval 0 Success
 ** @retval -32 No drive
 ** @retval -9 No such file
 ** @retval other Unknown error */
int rename_filepath(const char* oldpath, const char* newpath)
{
	if (has_working_drive != 1) return -32;
	switch (fs) {
#ifdef CONFIG_FAT12
		case fat12:
			return fat12_rename(oldpath, newpath);
#endif
		default:
			return -1;
	}
}

/** @retval 0 Success
 ** @retval -32 No drive
 ** @retval -9 Parent directory does not exist
 ** @retval other Unknown error */
int makedir(const char* path)
{
	if (has_working_drive != 1) return -32;
	switch (fs) {
#ifdef CONFIG_FAT12
		case fat12:
			return fat12_makedir(path);
#endif
		default:
			return -1;
	}
}

/** @retval 0 Success
 ** @retval -32 No drive
 ** @retval -11 Not empty
 ** @retval -9 No such directory
 ** @retval other Unknown error */
int removedir(const char* path)
{
	if (has_working_drive != 1) return -32;
	switch (fs) {
#ifdef CONFIG_FAT12
		case fat12:
			return fat12_removedir(path);
#endif
		default:
			return -1;
	}
}

/** @retval 0 Success
 ** @retval -32 No drive
 ** @retval -11 Being used
 ** @retval -9 No such file
 ** @retval other Unknown error */
int removefile(const char* filename)
{
	if (has_working_drive != 1) return -32;
	switch (fs) {
#ifdef CONFIG_FAT12
		case fat12:
			return fat12_removefile(filename);
#endif
		default:
			return -1;
	}
}

/** @retval -32 No drive
 ** @retval -5 Bad fileno
 ** @retval negative Unknown error
 ** @retval other Bytes written */
ptrdiff_t write_fileno(int fileno, const void* buf, size_t nbyte)
{
	if (fileno == LINEOUT_FILENO) {
		terminal_write((const char*)buf, nbyte);
		return nbyte;
	}
	if (has_working_drive != 1) return -32;
	switch (fs) {
#ifdef CONFIG_FAT12
		case fat12:
			return fat12_write(fileno, buf, nbyte);
#endif
		default:
			return -1;
	}
}

/** @retval -32 No drive
 ** @retval -5 Bad fileno
 ** @retval negative Unknown error
 ** @retval other Bytes read */
ptrdiff_t read_fileno(int fileno, void* buf, size_t count)
{
	if (has_working_drive != 1) return -32;
	if (fileno == LINEOUT_FILENO) return -5;
	switch (fs) {
#ifdef CONFIG_FAT12
		case fat12:
			return fat12_read(fileno, buf, count);
#endif
		default:
			return -1;
	}
}

/** @retval 0 Success
 ** @retval -32 No drive
 ** @retval -9 No such file/directory
 ** @retval other Unknown error */
int list_contents(const char* path, contents_t* fcontents, size_t count)
{
	if (has_working_drive != 1) return -32;
	switch (fs) {
#ifdef CONFIG_FAT12
		case fat12:
			return fat12_listcontent(path, fcontents, count);
#endif
		default:
			return -1;
	}
}

/** @retval 0 Empty
 ** @retval other Size */
size_t get_size(int fileno)
{
	switch (fs) {
#ifdef CONFIG_FAT12
		case fat12:
			return fat12_get_size(fileno);
#endif
		default:
			return -1;
	}
}
