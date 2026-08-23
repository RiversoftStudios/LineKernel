/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#ifndef __VGA_CONSOLE_H__
#define __VGA_CONSOLE_H__
#include "kconfig.h"
#ifdef CONFIG_VGA_CONSOLE

#include <stdint.h>
#include <stddef.h>
#include "bios_tools.h"
#include "LineKernel/linecolor_t.h"

extern size_t vga_terminal_row;
extern size_t vga_terminal_column;

void vga_terminal_cls(void);
void vga_terminal_enable_cursor(uint8_t cursor_start, uint8_t cursor_end);
void vga_terminal_update_cursor(int x, int y);
void vga_terminal_setcolor(linecolor_t fg_color, linecolor_t bg_color);
void vga_terminal_initialize(void);
void vga_terminal_putentryat(char c, uint8_t color, size_t x, size_t y);
void vga_terminal_putchar(char c);
void vga_terminal_newline(void);
void vga_terminal_backspace(void);
void vga_terminal_scroll(void);
void vga_terminal_get_cursor(uint8_t *x, uint8_t *y);

#endif
#endif
