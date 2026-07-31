/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#include "LineInput.h"

char get_char(void)
{
	char c;
	int ready;

#ifdef CONFIG_PS2_KEYBOARD
	ready = ps2_kbd_is_kbd_ready_to_read();
	if (ready == 0) {
		c = ps2_kbd_keyboard_read_scancode_to_ascii_right_now();
		if (c != '\0')
			return c;
	}
#endif

#ifdef CONFIG_SERIAL_CONSOLE
	ready = serial_received();
	if (ready != 0) {
		c = read_serial_right_now();
		c = serial_sane_control_codes(c);
		return c;
	}
#endif

#ifdef CONFIG_UART
	ready = get_uart_input();
	if (ready != 0) {
		c = ready;
		if (c == '\r')
			c = '\n';			// Workaround for newlines
		return c;
	}
#endif

	return '\0';
}

void keyboard_init(void)
{
#ifdef CONFIG_PS2_KEYBOARD
	ps2_kbd_keyboard_init();
#endif
}
