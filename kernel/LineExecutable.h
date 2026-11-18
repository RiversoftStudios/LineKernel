/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#ifndef __LINEEXECUTABLE_H__
#define __LINEEXECUTABLE_H__

#include "kconfig.h"
#include "elf.h"
#include "compilers.h"

/* Probably some more, but I won't do them all! */
#if CPU_BITS_32 == CPU_BITS
typedef Elf32_Ehdr Elf_Ehdr;
typedef Elf32_Shdr Elf_Shdr;
typedef Elf32_Chdr Elf_Chdr;
typedef Elf32_Phdr Elf_Phdr;
typedef Elf32_Sym Elf_Sym;
typedef Elf32_Syminfo Elf_Syminfo;
typedef Elf32_Half Elf_Half;
typedef Elf32_Word Elf_Word;
typedef Elf32_Addr Elf_Addr;
typedef Elf32_Off Elf_Off;
#elif CPU_BITS_64 == CPU_BITS
typedef Elf64_Ehdr Elf_Ehdr;
typedef Elf64_Shdr Elf_Shdr;
typedef Elf64_Chdr Elf_Chdr;
typedef Elf64_Phdr Elf_Phdr;
typedef Elf64_Sym Elf_Sym;
typedef Elf64_Syminfo Elf_Syminfo;
typedef Elf64_Half Elf_Half;
typedef Elf64_Word Elf_Word;
typedef Elf64_Addr Elf_Addr;
typedef Elf64_Off Elf_Off;
#else
#error "CPU bits unsupported!"
#endif

#ifdef CONFIG_ELF
int valid_elf_magic(Elf_Ehdr ehdr);
Elf_Addr get_elf_entry(Elf_Ehdr ehdr);
#endif

#endif
