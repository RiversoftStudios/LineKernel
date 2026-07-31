/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 River Games */

#ifndef __STR_H__
#define __STR_H__

#include <stddef.h>

size_t strlen(const char* str);
size_t strconcat(char* destination, const char* source, size_t size);
size_t strlcpy(char* __restrict dst, const char* __restrict src, size_t dsize);
char *strtok(char *s, const char *delim);
int strcmp(const char *s1, const char *s2);
void itoa(int N, char* str);
int sscanf(const char* buff, char* format, ...) __attribute__ ((format (scanf, 2, 3)));
void* memcpy(void* dest, const void* src, size_t n);
void* memset(void* s, int c, size_t n);
int memcmp(const void* s1, const void* s2, size_t n);

#endif

