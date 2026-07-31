/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

/* phoenixcoe got eaten by a grue
 * or he got mauled by a bear, idk
 * it could also possibly be SAM, he seems suspiciously quiet */
#include "kconfig.h"
#ifdef CONFIG_PS2_KEYBOARD
#include "ps2_keyboard.h"

static int is_shift_pressed = 0;

void ps2_kbd_wait_kbd_write(void)
{
	while (inb(PS2_KBD_STATUS_PORT) & PS2_KBD_STATUS_INPUT_FULL);
}

int ps2_kbd_is_kbd_ready_to_read(void)
{
	return (!(inb(PS2_KBD_STATUS_PORT) & PS2_KBD_STATUS_OUTPUT_FULL));
}

void ps2_kbd_wait_kbd_read(void)
{
	while (!(inb(PS2_KBD_STATUS_PORT) & PS2_KBD_STATUS_OUTPUT_FULL));
}

/* i think phoenix also found a way to get eaten by a troll, but that seems less likely than the other two options
 * best regards,
 * SAM */

void ps2_kbd_keyboard_init(void)
{
	ps2_kbd_wait_kbd_write();
	outb(PS2_KBD_DATA_PORT, PS2_KBD_CMD_SCAN_ON);
}

uint8_t ps2_kbd_keyboard_read_scancode(void)
{
	ps2_kbd_wait_kbd_read();
	return inb(PS2_KBD_DATA_PORT);
}

uint8_t ps2_kbd_keyboard_read_scancode_right_now(void)
{
	return inb(PS2_KBD_DATA_PORT);
}

static const char ascii_table[] = {
	0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
	'\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
	0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
	'\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' '
};

static const char shift_ascii_table[] = {
	0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
	'\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
	0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0,
	'|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' '
};

char ps2_kbd_keyboard_to_ascii(uint8_t scancode)
{
	if (scancode == 0x2A || scancode == 0x36) {
		is_shift_pressed = 1;
		return 0;
	}

	if (scancode == 0xAA || scancode == 0xB6) {
		is_shift_pressed = 0;
		return 0;
	}

	if (scancode & 0x80) {
		return 0;
	}

	if (scancode < sizeof(ascii_table)) {
		return is_shift_pressed ? shift_ascii_table[scancode] : ascii_table[scancode];
	}

	return 0;
}

char ps2_kbd_keyboard_read_scancode_to_ascii_right_now(void)
{
	uint8_t scancode = ps2_kbd_keyboard_read_scancode_right_now();
	char c = ps2_kbd_keyboard_to_ascii(scancode);

	return c;
}

char ps2_kbd_keyboard_read_scancode_to_ascii(void)
{
	uint8_t scancode = ps2_kbd_keyboard_read_scancode();
	char c = ps2_kbd_keyboard_to_ascii(scancode);

	return c;
}

#endif /* CONFIG_PS2_KEYBOARD */
