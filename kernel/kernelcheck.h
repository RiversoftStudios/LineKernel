/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#if defined(__linux__) || defined(__GNU__)
#error "Not cross-compiling!!!"
#endif

#if !defined(__i386__) && defined(ARCH_i386)
#error "Only compiles for i386!!!"
#endif

#include "kconfig.h"
