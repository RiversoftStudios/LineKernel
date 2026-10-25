/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#ifndef __POWER_H__
#define __POWER_H__

#include <stdint.h>
#include "compilers.h"

ATTRIBUTE_NORETURN void poweroff(void);
ATTRIBUTE_NORETURN void reboot(void);
ATTRIBUTE_NORETURN void halt(void);

#endif
