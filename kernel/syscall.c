/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 River Games */

#include "LineKernel/syscall/syscall_ids.h"
#include <stdint.h>
#include "LineRenderer.h"
#include "LineColor.h"
#include "LineFilesystem.h"

#define COMMAND2TABLE(name) [SYS_##name] = sys_##name

#define DEFINECALL(name, ...) int sys_##name(__VA_ARGS__); \
	int sys_##name(__VA_ARGS__)

typedef int (*syscall_t)(long, long, long, long, long, long);

DEFINECALL(print2, uintptr_t read)
{
	terminal_writestring((char*)read);
	return 0;
}

DEFINECALL(termcolor, linecolor_t fg_color, linecolor_t bg_color)
{
	terminal_setcolor(fg_color, bg_color);
	return 0;
}

DEFINECALL(termscolor, void)
{
	terminal_resetcolor();
	return 0;
}

DEFINECALL(termclear, void)
{
	terminal_cls();
	return 0;
}

DEFINECALL(termcursor, int x, int y)
{
	terminal_update_cursor(x, y);
	return 0;
}

DEFINECALL(close, int fileno)
{
	return close_fileno(fileno);
}

DEFINECALL(open, uintptr_t filename)
{
	return open_fileno((const char*)filename);
}

DEFINECALL(rename, uintptr_t oldpath, uintptr_t newpath)
{
	return rename_filepath((const char*)oldpath, (const char*)newpath);
}

DEFINECALL(makedir, uintptr_t path)
{
	return makedir((const char*)path);
}

DEFINECALL(removedir, uintptr_t path)
{
	return removedir((const char*)path);
}

DEFINECALL(write, int fileno, uintptr_t buf, size_t nbyte)
{
	return write_fileno(fileno, (const void*)buf, nbyte);
}

DEFINECALL(remove, uintptr_t filename)
{
	return removefile((const char*)filename);
}

DEFINECALL(read, int fileno, uintptr_t buf, size_t count)
{
	return read_fileno(fileno, (void*)buf, count);
}

DEFINECALL(list, uintptr_t path, uintptr_t fcontents, size_t count)
{
	return list_contents((const char*)path, (contents_t*)fcontents, count);
}

DEFINECALL(size, int fileno)
{
	return get_size(fileno);
}

void* syscall_table[] = {
	COMMAND2TABLE(print2),
	COMMAND2TABLE(termcolor),
	COMMAND2TABLE(termscolor),
	COMMAND2TABLE(termclear),
	COMMAND2TABLE(termcursor),
	COMMAND2TABLE(close),
	COMMAND2TABLE(open),
	COMMAND2TABLE(rename),
	COMMAND2TABLE(makedir),
	COMMAND2TABLE(removedir),
	COMMAND2TABLE(write),
	COMMAND2TABLE(remove),
	COMMAND2TABLE(read),
	COMMAND2TABLE(list),
	COMMAND2TABLE(size),
};
