/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#ifndef __PS2_KEYBOARD_H__
#define __PS2_KEYBOARD_H__
#include "kconfig.h"
#ifdef CONFIG_PS2_KEYBOARD

#include "bios_tools.h"
#include <stdint.h>

#define PS2_KBD_DATA_PORT 0x60
#define PS2_KBD_STATUS_PORT 0x64
#define PS2_KBD_COMMAND_PORT 0x64

#define PS2_KBD_CMD_SET_LED 0xED
#define PS2_KBD_CMD_ECHO 0xEE
#define PS2_KBD_CMD_SCAN_ON 0xF4
#define PS2_KBD_CMD_SCAN_OFF 0xF5
#define PS2_KBD_CMD_RESET 0xFF

#define PS2_KBD_STATUS_OUTPUT_FULL 0x01
#define PS2_KBD_STATUS_INPUT_FULL 0x02

void ps2_kbd_keyboard_init(void);
uint8_t ps2_kbd_keyboard_read_scancode(void);
uint8_t ps2_kbd_keyboard_read_scancode_right_now(void);
char ps2_kbd_keyboard_to_ascii(uint8_t scancode);
char ps2_kbd_keyboard_read_scancode_to_ascii(void);
char ps2_kbd_keyboard_read_scancode_to_ascii_right_now(void);
void ps2_kbd_wait_kbd_write(void);
int ps2_kbd_is_kbd_ready_to_read(void);
void ps2_kbd_wait_kbd_read(void);

#endif
#endif
