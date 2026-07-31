/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#ifndef __LINERENDERER_H__
#define __LINERENDERER_H__

#include "kconfig.h"
#include <stddef.h>
#include <stdint.h>
#include "str.h"
#ifdef CONFIG_SERIAL_CONSOLE
#include "serial.h"
#endif
#ifdef CONFIG_VGA_CONSOLE
#include "vga_console.h"
#endif
#ifdef CONFIG_UART
#include "uart.h"
#endif

void terminal_initialize(void);

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y);

void terminal_putc(int ch, void* stream);

void terminal_putchar(char c);

void terminal_newline(void);

void terminal_tab(void);

void terminal_backspace(void);

void terminal_write_for_char(const char data);

void terminal_write(const char* data, size_t size);

void terminal_writestring(const char* data);

typedef void (*writecharfunc_t)(char);

void terminal_writeintcustom(int data, writecharfunc_t func);

void terminal_writeint(int data);

void terminal_enable_cursor(void);

void terminal_update_cursor(int x, int y);

void terminal_cls(void);

#endif
