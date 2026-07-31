/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#ifndef __LINECOLOR_H__
#define __LINECOLOR_H__

#include "kconfig.h"
#include "version.h"
#include "LineKernel/linecolor_t.h"

#define DEFAULT_FOREGROUND COLOR_LIGHT_GREY
#define DEFAULT_BACKGROUND COLOR_BLACK

void terminal_setcolor(linecolor_t fg_color, linecolor_t bg_color);
void terminal_resetcolor();

#endif
