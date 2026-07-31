# How to port an architecture to LineKernel
1. Add your arch to [LineArch](https://github.com/RiverGamesStudios/LineArch).
    Create a `(arch).json` file with `CCPREFIX` (ex. `i686-elf-`), `CFLAGS` (arch specific CFLAGS), and `QEMU` (qemu for this machine).
    Then, add your `(arch).json` to `gen.sh`, adding it to both the `genmake` and `gensh` commands.
    Now, you can submit your patch. If it is properly formatted it will probably be submitted.
2. Create proper `boot.s`, `linker.ld` for arch.
3. Create a Makefile with `LDFLAGS` and `ARCH_OBJ`.
4. Create Kconfig for enabling/disabling features specific to the arch.
5. You probably want a console backend. Write one for serial or whatever your arch uses.
6. Get a kernel command line so you can specify what drive/what filesystem
7. Port syscalls

# Requirements
These functions/variables MUST be defined.

- `void reboot(void);` and `void poweroff(void);` function, even if it does nothing. This should be in `power.c`.
- `void halt(void);` function. This should halt the CPU, stalling it forever. This should be in `power.c`.

# Meta-arches
There are meta-arches which architectures are derivatives from.
`x86` is the only meta-arch as of now.

<!--
SPDX-License-Identifier: CC0-1.0
SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios
-->
