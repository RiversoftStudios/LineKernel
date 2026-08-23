/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

.section .init
.global _start
_start:
  la sp, stack_top
  call kernel_main

stack_top:
