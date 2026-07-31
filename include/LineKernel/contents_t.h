/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 River Games */

#ifndef _LineKernel___CONTENTS_T_H__
#define _LineKernel___CONTENTS_T_H__

#if defined(__LineCore__) && !defined(_CLineB___LINECORE_FILE_H__)
#error "Don't include this, include <LineCore/file.h>!"
#endif

enum contentstype {
	DIRECTORY,
	FILE
};

typedef enum contentstype contentstype_t;

struct contents {
	contentstype_t content_type;

	char* filename;
};

typedef struct contents contents_t;

#endif
