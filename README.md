# [<img src="https://rivergames.ca/src/img/LK_COLOUR.png" alt="LineKernel" width="512"/>](https://wiki.rivergames.ca/wiki/LineKernel)
[LineKernel](https://wiki.rivergames.ca/wiki/LineKernel) is a kernel unlike any other UNIX-Like kernel. LineKernel is a Monolithic kernel designed for real hardware, built from scratch, for the [LineCore Operating System](https://wiki.rivergames.ca/wiki/LineCoreOS).

This kernel is for the LineCoreOS [x86](https://wiki.rivergames.ca/wiki/History_of_LineCoreOS#x86) version.

## Features in Development
- Kernel

## Compiling
### Dependencies
+ [Cross-compile a GCC](https://wiki.osdev.org/GCC_Cross-Compiler) for the architecture needed (for `i386` it would be `i686-elf`).
+ QEMU
+ kconfig-frontends

### Makefile Instructions
Note that you can also use `make xconfig` for X config, `make allyesconfig` for All Yes config, `make allnoconfig` for All No config.
+ **For i386**
    + `make menuconfig ARCH=i386` - Configure LineKernel
    + `make ARCH=i386` - Compile using i686-elf for i386
    + `make run ARCH=i386` - Run an i386 kernel image in QEMU (faster)
    + `make iso ARCH=i386` - Make a bootable i386 ISO
    + `make run-iso ARCH=i386` - Run the i386 ISO in QEMU (slower, but more similar to running on real hardware)
+ **For x86_64**
    + `make menuconfig ARCH=x86_64` - Configure LineKernel
    + `make ARCH=x86_64` - Compile using x86_64-elf for x86_64
    + `make iso ARCH=x86_64` - Make a bootable x86_64 ISO
    + `make run-iso ARCH=x86_64` - Run the x86_64 ISO in QEMU
+ **For riscv64**
    + `make menuconfig ARCH=riscv64` - Configure LineKernel
    + `make ARCH=riscv64` - Compile using riscv64-elf for riscv64
    + `make run ARCH=riscv64` - Run an riscv64 kernel image in QEMU

<sup>(c) 2026 Riversoft Studios ~ LineKernel (Project Estuary)</sup>

<!--
SPDX-License-Identifier: CC0-1.0
SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios
-->
