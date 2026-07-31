/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#include "panic.h"

char* error_art[] = {
/* https://www.asciiart.eu/art/3f48d10c283c85d8 */
		"   +--------------+\n"
		"   |.------------.|\n"
		"   ||   Panic!   ||\n"
		"   ||   in the   ||\n"
		"   ||   Kernel   ||\n"
		"   ||            ||\n"
		"   |+------------+|\n"
		"   +-..--------..-+\n"
		"   .--------------.\n"
		"  / /============\\ \\\n"
		" / /==============\\ \\\n"
		"/____________________\\\n"
		"\\____________________/\n",

/* https://www.asciiart.eu/art/a5b14e0367b64925 */
		"BBBBBBBBBBBBBBBBBBBBBBBBBBB\n"
		"BMB-Riversoft Studios---------B B\n"
		"BBB---------------------BBB\n"
		"BBB-----LineCoreOS------BBB\n"
		"BBB---------------------BBB\n"
		"BBB--------installation-BBB\n"
		"BBB----------disk #1/3--BBB\n"
		"BBBBBBBBBBBBBBBBBBBBBBBBBBB\n"
		"BBBBB++++++++++++++++BBBBBB\n"
		"BBBBB++BBBBB+++++++++BBBBBB\n"
		"BBBBB++BBBBB+++++++++BBBBBB\n"
		"BBBBB++BBBBB+++++++++BBBBBB\n"
		"BBBBB++++++++++++++++BBBBBB\n",

/* https://www.asciiart.eu/art/37bc7fd33d1e26f6 */
		" ______________\n"
		"||            ||\n"
		"||            ||\n"
		"||            ||\n"
		"||            ||\n"
		"||____________||\n"
		"|______________|\n"
		" \\\\############\\\\\n"
		"  \\\\############\\\\\n"
		"   \\      ____    \\   \n"
		"    \\_____\\___\\____\\\n"

#ifdef ARCH_riscv64
,
		" ____________________________\n"
		"< This seems kinda RISC-y... >\n"
		" ----------------------------\n"
		"        \\   ^__^\n"
		"         \\  (oo)\\_______\n"
		"            (__)\\       )\\/\\\n"
		"                ||----w |\n"
		"                ||     ||\n"
#endif
};

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
