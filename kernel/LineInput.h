/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#ifndef __LINEINPUT_H__
#define __LINEINPUT_H__

#include "kconfig.h"

#ifdef CONFIG_PS2_KEYBOARD
#include "ps2_keyboard.h"
#endif

#ifdef CONFIG_SERIAL_CONSOLE
#include "serial.h"
#endif

#ifdef CONFIG_UART
#include "uart.h"
#endif

char get_char(void);
void keyboard_init(void);

#endif
