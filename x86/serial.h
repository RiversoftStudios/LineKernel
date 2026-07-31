/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#ifndef __SERIAL_H__
#define __SERIAL_H__
#include "kconfig.h"

#ifdef CONFIG_SERIAL_CONSOLE
#include "bios_tools.h"

#define PORT 0x3f8

int init_serial(void);

int serial_received(void);

char read_serial_right_now(void);

char read_serial(void);

char serial_sane_control_codes(const char c);

int is_transmit_empty(void);

void write_serial(char a);
#endif

#endif
