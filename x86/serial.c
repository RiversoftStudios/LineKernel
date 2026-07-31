/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#include "kconfig.h"

#ifdef CONFIG_SERIAL_CONSOLE
#include "serial.h"

int init_serial(void)
{
	outb(PORT + 1, 0x00);
	outb(PORT + 3, 0x80);
	outb(PORT + 0, 0x03);
	outb(PORT + 1, 0x00);
	outb(PORT + 3, 0x03);
	outb(PORT + 2, 0xC7);
	outb(PORT + 4, 0x0B);
	outb(PORT + 4, 0x1E);
	outb(PORT + 0, 0xAE);

	if (inb(PORT + 0) != 0xAE) {
		return 1;
	}

	outb(PORT + 4, 0x0F);
	return 0;
}

int serial_received(void)
{
	return inb(PORT + 5) & 1;
}

char serial_sane_control_codes(const char c)
{
	if (c == '\r')
		return '\n';
	else
		return c;
}

char read_serial_right_now(void)
{
	return inb(PORT);
}

char read_serial(void)
{
	while (serial_received() == 0);

	return read_serial_right_now();
}

int is_transmit_empty(void)
{
	return inb(PORT + 5) & 0x20;
}

void write_serial(char a)
{
	while (is_transmit_empty() == 0);

	outb(PORT, a);
}
#endif
