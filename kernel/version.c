/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#include "version.h"

void we_are_running(void)
{
	terminal_writestring("Target: " ARCH);
	terminal_writestring("\n\n");
}

void nothing(void)
{
	return;
}
