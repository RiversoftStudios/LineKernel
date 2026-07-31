/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#ifndef __LINEKERNEL_H__
#define __LINEKERNEL_H__

#include "LineDrive.h"
#include "LineFilesystem.h"

void initialize_start(void);
void kernel_main(void);
void handle_single_argument(char* arg);
void parse_cmdline(char* cmdline);

extern char cmdline_str[100];

extern drivesformat_t drive;
extern filesystemformat_t fs;
extern int has_working_drive;

#endif
