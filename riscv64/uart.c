/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#include "kconfig.h"
#ifdef CONFIG_UART
#include "uart.h"

void uart_init(void)
{
	volatile uint8_t* ptr = (uint8_t *) UART_ADDR;

	/* Set word length to 8 (LCR[1:0]) */
	const uint8_t LCR = 0x3;

	ptr[3] = LCR;

	/* Enable FIFO (FCR[0]) */
	ptr[2] = 0x1;

	/* Enable receiver buffer interrupts (IER[0]) */
	ptr[1] = 0x1;
}

void uart_putchar(char c)
{
	*(uint8_t *) UART_ADDR = c;
}

char get_uart_input(void)
{
	return *(uint8_t *) UART_ADDR;
}

char read_uart(void)
{
	char c = 0;

	while (c == 0) {
		c = get_uart_input();
	}
	return c;
}

#endif
