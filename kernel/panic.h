/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#ifndef __PANIC_H__
#define __PANIC_H__

#include "LineRenderer.h"
#include "power.h"
#include "str.h"
#include "printf.h"
#include "LineRandom.h"

char* random_error_message(void);
void panic_(const char* file, int line, const char* format, ...);

#define panic(format, ...) panic_(__FILE__, __LINE__, format __VA_OPT__(,) __VA_ARGS__)

#endif
