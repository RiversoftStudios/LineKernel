/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#include "LineColor.h"
#ifdef CONFIG_VGA_CONSOLE
#include "vga_console.h"
#endif
#ifdef CONFIG_SERIAL_CONSOLE
#include "serial.h"
#endif
#ifdef ARCH_riscv64
#include "sbi.h"
#endif
#include "str.h"

const char* ansi_fg_colors[] = {
	"\033[30m", "\033[34m", "\033[32m", "\033[36m",
	"\033[31m", "\033[35m", "\033[33m", "\033[37m",
	"\033[90m", "\033[94m", "\033[92m", "\033[96m",
	"\033[91m", "\033[95m", "\033[93m", "\033[97m"
};

const char* ansi_bg_colors[] = {
	"\033[40m", "\033[44m", "\033[42m", "\033[46m",
	"\033[41m", "\033[45m", "\033[43m", "\033[47m",
	"\033[100m", "\033[104m", "\033[102m", "\033[106m",
	"\033[101m", "\033[105m", "\033[103m", "\033[107m"
};

void terminal_setcolor(linecolor_t fg_color, linecolor_t bg_color)
{
#ifdef CONFIG_VGA_CONSOLE
	vga_terminal_setcolor(fg_color, bg_color);
#endif
	for (size_t i = 0; i < strlen(ansi_fg_colors[fg_color]); i++) {
#ifdef CONFIG_SERIAL_CONSOLE
		write_serial(ansi_fg_colors[fg_color][i]);
#endif
#ifdef ARCH_riscv64
		sbi_putchar(ansi_fg_colors[fg_color][i]);
#endif
	}
	for (size_t i = 0; i < strlen(ansi_bg_colors[bg_color]); i++) {
#ifdef CONFIG_SERIAL_CONSOLE
		write_serial(ansi_bg_colors[bg_color][i]);
#endif
#ifdef ARCH_riscv64
		sbi_putchar(ansi_bg_colors[bg_color][i]);
#endif
	}
}

void terminal_resetcolor(void)
{
#ifdef CONFIG_VGA_CONSOLE
	vga_terminal_setcolor(DEFAULT_FOREGROUND, DEFAULT_BACKGROUND);
#endif
#ifdef CONFIG_SERIAL_CONSOLE
	write_serial('\033');
	write_serial('[');
	write_serial('0');
	write_serial('m');
#endif
}
