/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdint.h>

void timer_init(uint32_t frequency);
void sleep_ms(uint32_t ms);
uint32_t timer_get_ticks(void);

#endif
