/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#include "LineExecutable.h"
#include "LineMemory.h"
#include "str.h"

#ifdef CONFIG_ELF

int valid_elf_magic(Elf_Ehdr ehdr)
{
	if (ehdr.e_ident[EI_MAG0] != ELFMAG0 || ehdr.e_ident[EI_MAG1] != ELFMAG1
		|| ehdr.e_ident[EI_MAG2] != ELFMAG2 || ehdr.e_ident[EI_MAG3] != ELFMAG3) {
		return 0;
	}
	return 1;
}

/** PIE-independent, you will have to check that yourself.
 ** @retval -1 Failure, invalid header 
 ** @retval other Address */
Elf_Addr get_elf_entry(Elf_Ehdr ehdr)
{
	if (valid_elf_magic(ehdr))
		return ehdr.e_entry;
	return -1;
}

#endif
