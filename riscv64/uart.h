/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#ifndef __UART_H__
#define __UART_H__

#include "kconfig.h"
#ifdef CONFIG_UART

#ifdef CONFIG_VIRT_MACHINE
#define UART_ADDR 0x10000000
#endif

#include <stdint.h>
#include <stddef.h>
void uart_init(void);
void uart_putchar(char c);
char get_uart_input(void);
char read_uart(void);

#endif
#endif
