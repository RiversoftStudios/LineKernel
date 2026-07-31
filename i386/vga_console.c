/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#include "vga_console.h"
#ifdef CONFIG_VGA_CONSOLE

static inline uint8_t vga_entry_color(linecolor_t fg, linecolor_t bg)
{
	return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color)
{
	return (uint16_t) uc | (uint16_t) color << 8;
}

#define VGA_MEMORY	0xB8000
#define VGA_LINES	25
#define VGA_WIDTH	80

size_t vga_terminal_row;
size_t vga_terminal_column;
uint8_t vga_terminal_color;
volatile uint16_t* vga_terminal_buffer = (uint16_t *) VGA_MEMORY;

void vga_terminal_cls(void)
{
	vga_terminal_row = 0;
	vga_terminal_column = 0;

	for (size_t y = 0; y < VGA_LINES; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;

			vga_terminal_buffer[index] = vga_entry(' ', vga_terminal_color);
		}
	}
}

void vga_terminal_setcolor(linecolor_t fg_color, linecolor_t bg_color)
{
	vga_terminal_color = vga_entry_color(fg_color, bg_color);
}

void vga_terminal_enable_cursor(uint8_t cursor_start, uint8_t cursor_end)
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);

	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}

uint16_t vga_terminal_cursor_position(void)
{
	uint16_t pos = 0;

	outb(0x3D4, 0x0F);
	pos |= inb(0x3D5);
	outb(0x3D4, 0x0E);
	pos |= ((uint16_t) inb(0x3D5)) << 8;
	return pos;
}

void vga_terminal_update_cursor(int x, int y)
{
	uint16_t pos = y * VGA_WIDTH + x;

	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

void vga_terminal_initialize(void)
{
	vga_terminal_color = vga_entry_color(COLOR_LIGHT_GREY, COLOR_BLACK);

	vga_terminal_cls();
}

void vga_terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
	const size_t index = y * VGA_WIDTH + x;

	vga_terminal_buffer[index] = vga_entry(c, color);
}

void vga_terminal_scroll(void)
{
	for (size_t y = 1; y < VGA_LINES; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			size_t dest_index = (y - 1) * VGA_WIDTH + x;
			size_t src_index = y * VGA_WIDTH + x;

			vga_terminal_buffer[dest_index] = vga_terminal_buffer[src_index];
		}
	}

	for (size_t x = 0; x < VGA_WIDTH; x++) {
		size_t index = (VGA_LINES - 1) * VGA_WIDTH + x;

		vga_terminal_buffer[index] = vga_entry(' ', vga_terminal_color);
	}
}

void vga_terminal_putchar(char c)
{
	vga_terminal_putentryat(c, vga_terminal_color, vga_terminal_column, vga_terminal_row);
	if (++vga_terminal_column == VGA_WIDTH) {
		vga_terminal_column = 0;
		if (++vga_terminal_row == VGA_LINES) {
			vga_terminal_scroll();
			vga_terminal_row = VGA_LINES - 1;
		}
	}
	vga_terminal_update_cursor(vga_terminal_column, vga_terminal_row);
}

void vga_terminal_newline(void)
{
	vga_terminal_column = 0;
	if (++vga_terminal_row == VGA_LINES) {
		vga_terminal_scroll();
		vga_terminal_row = VGA_LINES - 1;
	}
	vga_terminal_update_cursor(vga_terminal_column, vga_terminal_row);
}

void vga_terminal_backspace(void)
{
	if (vga_terminal_column == 0 && vga_terminal_row == 0)
		return;

	if (vga_terminal_column == 0) {
		vga_terminal_row--;
		vga_terminal_column = VGA_WIDTH - 1;
	} else {
		vga_terminal_column--;
	}

	const size_t index = vga_terminal_row * VGA_WIDTH + vga_terminal_column;

	vga_terminal_buffer[index] = vga_entry(' ', vga_terminal_color);
	vga_terminal_update_cursor(vga_terminal_column, vga_terminal_row);
}

#endif
