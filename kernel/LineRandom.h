/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#ifndef __LINERANDOM_H__
#define __LINERANDOM_H__

#include "kconfig.h"
#include <stdint.h>
#include "LineKernel/rand_max.h"

int rand_crand(void);

int rand(void);

typedef enum {
	RAND_CRAND,
} randtype_t;

randtype_t get_rand_type(void);

#endif
