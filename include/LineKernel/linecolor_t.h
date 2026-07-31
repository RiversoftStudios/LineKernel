/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#ifndef _LineKernel___LINECOLOR_T_H__
#define _LineKernel___LINECOLOR_T_H__

#if defined(__LineCore__) && !defined(_CLineB___LINECORE_CONSOLE_H__)
#error "Don't include this, include <LineCore/console.h>!"
#endif

enum linecolor {
	COLOR_BLACK = 0,
	COLOR_BLUE,
	COLOR_GREEN,
	COLOR_CYAN,
	COLOR_RED,
	COLOR_MAGENTA,
	COLOR_BROWN,
	COLOR_LIGHT_GREY,
	COLOR_DARK_GREY,
	COLOR_LIGHT_BLUE,
	COLOR_LIGHT_GREEN,
	COLOR_LIGHT_CYAN,
	COLOR_LIGHT_RED,
	COLOR_LIGHT_MAGENTA,
	COLOR_LIGHT_YELLOW,
	COLOR_WHITE
};

typedef enum linecolor linecolor_t;

#endif
