/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#include "LineRenderer.h"

void terminal_enable_cursor(void)
{
#ifdef CONFIG_VGA_CONSOLE
	vga_terminal_enable_cursor(
#if defined(CONFIG_CURSOR_SMALL)
		14,
#elif defined(CONFIG_CURSOR_FULL)
		0,
#endif
		15);
#endif
}

void terminal_update_cursor(int x, int y)
{
#ifdef CONFIG_VGA_CONSOLE
	vga_terminal_update_cursor(x, y);
	vga_terminal_row = y;
	vga_terminal_column = x;
#endif
#ifdef CONFIG_SERIAL_CONSOLE
	/* \033[<Y>;<X>H */
	write_serial('\033');
	write_serial('[');
	terminal_writeintcustom(y + 1, write_serial);
	write_serial(';');
	terminal_writeintcustom(x + 1, write_serial);
	write_serial('H');
#endif
#ifdef ARCH_riscv64
	/* \033[<Y>;<X>H */
	sbi_putchar('\033');
	sbi_putchar('[');
	terminal_writeintcustom(y + 1, sbi_putchar);
	sbi_putchar(';');
	terminal_writeintcustom(x + 1, sbi_putchar);
	sbi_putchar('H');
#endif
}

void terminal_initialize(void)
{
#ifdef CONFIG_VGA_CONSOLE
	vga_terminal_initialize();
#endif
#ifdef CONFIG_SERIAL_CONSOLE
	init_serial();
#endif
	terminal_enable_cursor();
}

void terminal_putchar(char c)
{
#ifdef CONFIG_VGA_CONSOLE
	vga_terminal_putchar(c);
#endif
#ifdef CONFIG_SERIAL_CONSOLE
	write_serial(c);
#endif
#ifdef ARCH_riscv64
	sbi_putchar(c);
#endif
}

void terminal_newline(void)
{
#ifdef CONFIG_VGA_CONSOLE
	vga_terminal_newline();
#endif
#ifdef CONFIG_SERIAL_CONSOLE
	write_serial('\r');
	write_serial('\n');
#endif
#ifdef ARCH_riscv64
	sbi_putchar('\r');
	sbi_putchar('\n');
#endif
}

void terminal_backspace(void)
{
#ifdef CONFIG_VGA_CONSOLE
	vga_terminal_backspace();
#endif
#ifdef CONFIG_SERIAL_CONSOLE
	write_serial('\b');
	write_serial(' ');
	write_serial('\b');
#endif
#ifdef ARCH_riscv64
	sbi_putchar('\b');
	sbi_putchar(' ');
	sbi_putchar('\b');
#endif
}

void terminal_tab(void)
{
	terminal_putchar(' ');
	terminal_putchar(' ');
	terminal_putchar(' ');
	terminal_putchar(' ');
}

void terminal_cls(void)
{
#ifdef CONFIG_VGA_CONSOLE
	vga_terminal_cls();
#endif
#ifdef CONFIG_SERIAL_CONSOLE
	/* \033[2J */
	write_serial('\033');
	write_serial('[');
	write_serial('2');
	write_serial('J');
	/* \033[1;1H */
	write_serial('\033');
	write_serial('[');
	write_serial('1');
	write_serial(';');
	write_serial('1');
	write_serial('H');
#endif
#ifdef ARCH_riscv64
	/* \033[2J */
	sbi_putchar('\033');
	sbi_putchar('[');
	sbi_putchar('2');
	sbi_putchar('J');
	/* \033[1;1H */
	sbi_putchar('\033');
	sbi_putchar('[');
	sbi_putchar('1');
	sbi_putchar(';');
	sbi_putchar('1');
	sbi_putchar('H');
#endif
}

void terminal_write_for_char(const char c)
{
	if (c == '\b')
		terminal_backspace();
	else if (c == '\n')
		terminal_newline();
	else if (c == '\t')
		terminal_tab();
	else if (c != 0)
		terminal_putchar(c);
}

/* needed for printf */
void _putchar(char character)
{
	terminal_write_for_char(character);
}

void terminal_write(const char* data, size_t size)
{
	for (size_t i = 0; i < size; i++)
		terminal_write_for_char(data[i]);
}

void terminal_writestring(const char* data)
{
	terminal_write(data, strlen(data));
}

void terminal_writeintcustom(int data, writecharfunc_t func)
{
	if (data < 0) {
		func('-');
		data = -data;
	}
	if (data / 10) {
		terminal_writeintcustom(data / 10, func);
	}
	func((data % 10) + '0');
}

void terminal_writeint(int data)
{
	if (data < 0) {
		terminal_write_for_char('-');
		data = -data;
	}
	if (data / 10) {
		terminal_writeint(data / 10);
	}
	terminal_write_for_char((data % 10) + '0');
}
