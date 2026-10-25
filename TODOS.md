Results I got while simply just searching for "TODO".

- [`gdt.c`](i386/gdt.c) - TODO: This is one of the worst hacks in the kernel. We need to fix this!
- [`timer.c`](i386/timer.c) - TODO: Make this more portable(?)
- [`LineDrive.c`](kernel/LineDrive.c) - TODO: How many bits/what type should out_buf be?
- [`LineInput.c`](kernel/LineInput.c) - TODO: Some of this stuff probably needs fixing to work better on Real Hardware.
- [`LineMemory.c`](kernel/LineMemory.c) - TODO: Test this more.
- [`power.c`](riscv64/power.c) - TODO: Some of these values are specific to `virt`.
- [`config2header.c`](tools/config2header.c) - TODO: Dynamic buffer
- [`config2makefile.c`](tools/config2makefile.c) - TODO: Dynamic buffer
- [`Kconfig`](x86/Kconfig) - TODO: move console here
- [`power.c`](x86/power.c) - TODO: ACPI
- [`linker.ld`](x86_64/linker.ld) - TODO: Fix x86_64 support, for now we just provide nothing

These are not as essential but some things to consider.
Also see: [Finishing LineKernel](TODO)

<!--
SPDX-License-Identifier: CC0-1.0
SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios
-->
