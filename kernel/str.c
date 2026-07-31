/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#include "str.h"
#include "strlcpy.c"
#include "strtok.c"
#include "strcmp.c"
#include "c_scan.c"

size_t strlen(const char* str)
{
	size_t len = 0;

	while (str[len])
		len++;
	return len;
}

size_t strconcat(char* destination, const char* source, size_t size)
{
	char* ptr = destination + strlen(destination);

	while (ptr - destination < (long int)size - 1 && *source) {
		*ptr = *source;
		ptr++;
		source++;
	}
	*ptr = '\0';
	return ptr - destination;
}

void itoa(int N, char* str)
{
	int i = 0;
	int j = 0;
	int k = 0;
	int sign = N;

	if (N == 0) {
		str[0] = '0';
		str[1] = '\0';
		return;
	}

	if (N < 0)
		N = -N;

	while (N > 0) {
		str[i++] = N % 10 + '0';
		N /= 10;
	}

	if (sign < 0) {
		str[i++] = '-';
	}

	str[i] = '\0';

	for (j = 0, k = i - 1; j < k; j++, k--) {
		char temp = str[j];

		str[j] = str[k];
		str[k] = temp;
	}
}

void* memcpy(void* dest, const void* src, size_t n)
{
	char* d = dest;
	const char* s = src;
	while (n--) {
		*d++ = *s++;
	}
	return dest;
}

void* memset(void* s, int c, size_t n)
{
	char* p = s;
	while (n--) {
		*p++ = (char)c;
	}
	return s;
}

int memcmp(const void* s1, const void* s2, size_t n)
{
	const unsigned char* p1 = s1;
	const unsigned char* p2 = s2;
	while (n--) {
		if (*p1 != *p2) {
			return *p1 - *p2;
		}
		p1++;
		p2++;
	}
	return 0;
}

