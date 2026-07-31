/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#include "LineRandom.h"
#include "printf.h"

/* Obviously none of this is designed for security */

/* https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1570.pdf */
static unsigned long int next = 'L' + 'I' + 'N' + 'E' + 'C' + 'O' + 'R' + 'E';

int rand_crand(void)
{
	next = next * 1103515245 + 12345;
	return (next / (RAND_MAX * 2 + 2)) % (RAND_MAX + 1);
}
/* https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1570.pdf */

randtype_t get_rand_type(void)
{
	return RAND_CRAND;
}

int rand(void)
{
	switch (get_rand_type()) {
		case RAND_CRAND:
			return rand_crand();
		default:
			return 0;
		}
}
