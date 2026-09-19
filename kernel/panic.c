/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#include "panic.h"

#include "panic.art"

#define ERROR_ART (sizeof(error_art) / sizeof(char *))

char* random_error_message(void)
{
	int fake_rand = rand();

	if (fake_rand) {
		return error_art[fake_rand % ERROR_ART];
	} else {
		return "\n";
	}
}

void panic_(const char* file, int line, const char* format, ...)
{
	char linestr[30];

	terminal_writestring(random_error_message());
	terminal_writestring("KERNEL PANIC!\n");
	/* Where? */
	terminal_writestring("Where? ");
	terminal_writestring(file);
	terminal_writestring(":");
	itoa(line, linestr);
	terminal_writestring(linestr);
	terminal_writestring("\n");
	/* Reason: */
	terminal_writestring("Reason:\n");
	va_list arg;

	va_start(arg, format);
	vprintf(format, arg);
	va_end(arg);
	halt();
}
